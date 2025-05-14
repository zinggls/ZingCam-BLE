
#include "project.h"
#include <stdio.h>
#include <ZFrame.h>
#include <UartBuf.h>
#include <Zing.h>
#include <i2cs.h>
#include "imu.h"
#include "NoLog.h"
#include "bcc.h"
#include "icd.h"
#include "Peripheral.h"
#include "git_describe.h"
#include "versionInfo.h"
#include "FlashRow.h"
#include "hgate.h"

#define BUTTON_HOLD_TIME_SEC  3   //3 seconds

static void setBccVersion()
{
    Version ver;
    memset(&ver,0,sizeof(Version));
    strcpy(ver.info,GIT_DESCRIBE);
    memcpy(getI2CReadBuffer()+I2C_IVF_READ_BUFFER_SIZE,ver.info,VERSION_SIZE);
}

static CYBLE_API_RESULT_T sendITF(char itf)
{
    // Use the characteristic index to obtain the attribute handle    
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle = cyBle_customCServ[CYBLE_CUSTOMC_CUSTOM_SERVICE_SERVICE_INDEX]
                                                .customServChar[CYBLE_CUSTOMC_CUSTOM_SERVICE_ZXX_CHAR_INDEX]
                                                .customServCharHandle;

    CYBLE_GATTC_WRITE_REQ_T writeReq;
    writeReq.attrHandle = attrHandle;
    writeReq.value.val = (uint8_t*)&itf;
    writeReq.value.len = sizeof(char);

    return CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeReq);
}

static uint32 interferenceCount = 0;
static bool bITF = false;

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    // Parsing the values into the structure
    if (!parse(getZcdFrame(),buf)) {
#ifdef ZXX_DEBUG
        L("Parsing Error\r\n");
        L("Received: ");
        L(zing_status);
        L("\r\n");
#endif
    }else{
        if(getZcdFrame()->itf=='Y') {
            interferenceCount++;
        }else{
            interferenceCount = 0;
        }
        
        if(bITF == false && interferenceCount>=itfCriteria) {
            //Interference appeared
            if(ivfCom.wirelessVideoChannelMode==0x01 ) { //자동 채널 설정 모드
                sendITF('Y');
                SPDT_Write(~SPDT_Read());
            }
            bITF = true;
        } else if(bITF==false && interferenceCount < itfCriteria) {
            //Negligible interference (DO NOTHING)
        } else if(bITF==true && interferenceCount >= itfCriteria) {
            //Still under interference (DO NOTHING)
        } else if(bITF==true && interferenceCount < itfCriteria) {
            //Interference disappeared
            bITF = false;
        }
    }
}

static uint8 sof = 1;                       //Euler angles as default
static uint8_t currentImuOutputType = 0x0;  //default
static uint8_t currentImuCalibrate = 0x0;   //default

static void processImuOutputType(uint8_t imuOutputType)
{
    if(imuOutputType != currentImuOutputType) {
        /*
        SET OUTPUT FORMAT, 1:Euler Angles, 2:Quaternion
        */
        sof = imuOutputType+1;  //ICD, 0x00 : Euler Angle (Default) 0x01 : Quaternion
        
        ImuFrame_setSof(sof);
        UART_IMU_InitializeOutputFormat(sof);
        currentImuOutputType = imuOutputType;
    }
}

static void processImuCalibrate(uint8_t imuCalibrate)
{
    if(imuCalibrate != currentImuCalibrate) {
        /*
        0x00 : (default)
        0x01 : 자이로 보정
        0x02 : 가속도 보정
        0x03 : 지자계 보정
        0x04 : 지자계 보정 종료
        0x05 : 지자기 Off
        0x06 : 지자기 On
        0x07 : 지자기 능동형 On
        */
        
        bool bProcessed = false;
        switch(imuCalibrate) {
        case 0x0:
            break;
        case 0x1:   //자이로 보정
            UART_IMU_UartPutString("<cg>");
            bProcessed = true;
            break;
        case 0x2:   //가속도 보정
            UART_IMU_UartPutString("<cas>");
            bProcessed = true;
            break;
        case 0x3:   //지자계 보정
            UART_IMU_UartPutString("<cmf>");
            bProcessed = true;
            break;
        case 0x4:   //지자계 보정 종료
            UART_IMU_UartPutString(">");
            bProcessed = true;
            break;
        case 0x5:   //지자기 Off
            UART_IMU_UartPutString("<sem0>");
            bProcessed = true;
            break;
        case 0x6:   //지자기 On
            UART_IMU_UartPutString("<sem1>");
            bProcessed = true;
            break;
        case 0x7:   //지자기 능동형 On
            UART_IMU_UartPutString("<sem2>");
            bProcessed = true;
            break;
        default:
            break;
        }
        currentImuCalibrate = imuCalibrate;
    }
}

static void processImuCommand(uint8_t imuOutputType,uint8_t imuCalibrate)
{
    processImuOutputType(imuOutputType);
    processImuCalibrate(imuCalibrate);
}

static uint8_t curLevel = 0x0;  //default

static void RxHemt1(uint8_t level)
{
    switch(level)
    {
        case 0x0: //default
            HGATE_Con1_Write(0x00);
            break;
        case 0x1:
            HGATE_Con1_Write(0x04);
            break;
        case 0x2:
            HGATE_Con1_Write(0x01);
            break;
        case 0x3:
            HGATE_Con1_Write(0x08);
            break;
        case 0x4:
            HGATE_Con1_Write(0x02);
            break;
        case 0x5:
            HGATE_Con1_Write(0x0C);
            break;
        case 0x6:
            HGATE_Con1_Write(0x03);
            break;
        case 0x7:
            HGATE_Con1_Write(0x07);
            break;
        case 0x8:
            HGATE_Con1_Write(0x0B);
            break;
        case 0x9:
            HGATE_Con1_Write(0x0F);
            break;
        default:
            break;
    }
}

static void RxHemt2(uint8_t level)
{
    switch(level)
    {
        case 0x0: //default
            HGATE_Con2_1_Write(0x00);
            HGATE_Con2_2_Write(0x00);
            break;
        case 0x1:
            HGATE_Con2_1_Write(0x00);
            HGATE_Con2_2_Write(0x01);
            break;
        case 0x2:
            HGATE_Con2_1_Write(0x01);
            HGATE_Con2_2_Write(0x00);
            break;
        case 0x3:
            HGATE_Con2_1_Write(0x00);
            HGATE_Con2_2_Write(0x02);
            break;
        case 0x4:
            HGATE_Con2_1_Write(0x02);
            HGATE_Con2_2_Write(0x00);
            break;
        case 0x5:
            HGATE_Con2_1_Write(0x00);
            HGATE_Con2_2_Write(0x03);
            break;
        case 0x6:
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x00);
            break;
        case 0x7:
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x01);
            break;
        case 0x8:
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x02);
            break;
        case 0x9:
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x03);
            break;
        default:
            break;
    }
}

static void RxHemtPower(uint8_t level)
{
    RxHemt1((level&0xf0)>>4);
    RxHemt2(level&0xf);
    getI2CReadBuffer()[ICD_RX_POWER_LEVEL_OFFSET] = level;
    
#ifdef RX_HEMT_POWER_DEBUG
    uint8 org = SPDT_Read();
    for(uint8_t i=0;i<level;i++) {
        SPDT_Write(~SPDT_Read());
        CyDelay(500);
        SPDT_Write(org);
        CyDelay(500);
    }
#else
    //HemtPower변경 명령이 먹었는지 확인할 수 있도록 단한번만 깜빡이도록 한다 (100ms)
    uint8 org = SPDT_Read();
    SPDT_Write(~SPDT_Read());
    CyDelay(100);
    SPDT_Write(org);
#endif
}

void RxHemtPower_Hanwha(uint8_t level)
{
    switch(level)
    {
        case 0x0: //default
        case 0x1:
            HGATE_Con1_Write(0x03);
            HGATE_Con2_1_Write(0x00);
            HGATE_Con2_2_Write(0x03);
            break;
        case 0x2:
            HGATE_Con1_Write(0x03);
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x00);
            break;
        case 0x3:
            HGATE_Con1_Write(0x0B);
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x01);
            break;
        case 0x4:
            HGATE_Con1_Write(0x0F);
            HGATE_Con2_1_Write(0x03);
            HGATE_Con2_2_Write(0x03);
            break;
        default:
            break;
    }
}

static void processPower(uint8_t level)
{
    if(level!=curLevel) {
#ifdef HANWHA
        RxHemtPower_Hanwha(level);
#else
        RxHemtPower(level);
#endif
        curLevel = level;
    }
}

static short toShort(const char *data)
{
    return (short)((data[1] << 8) | data[0]);
}

static void onImuFrame(const ImuFrame *imu)
{
    IMU i;
    
    if(sof==1) {    //Euler
        i.imu1 = 0x0000;
        i.imu2 = toShort(imu->data+2);
        i.imu3 = toShort(imu->data+4);
        i.imu4 = toShort(imu->data+6);
        i.imu5 = toShort(imu->data+8);
        i.imuChecksum = toShort(imu->data+10);
    }
    
    if(sof==2) {    //Quaternion
        i.imu1 = toShort(imu->data+2);
        i.imu2 = toShort(imu->data+4);
        i.imu3 = toShort(imu->data+6);
        i.imu4 = toShort(imu->data+8);
        i.imu5 = toShort(imu->data+10);
        i.imuChecksum = toShort(imu->data+12);
    }
    
    memcpy(getI2CReadBuffer()+IMU_RX_OFFSET,&i,IMU_FRAME_SIZE); //무선영상 수신기IMU 34
}

static void clearZcdZingInfo(ZCD_FRAME *z)
{
    z->kind = 0;
    z->usb = 0;
    z->bnd = ' ';    
    z->ppid = 0;
    z->devid = 0;
    z->fmt = 0;
    z->idx = 0;
    z->trt = ' ';
    z->ack = ' ';
    z->ppc = ' ';
    z->run = ' ';
    z->txid = 0;
    z->rxid = 0;
    z->cnt = 0;
    z->pos = 0;
    z->fps = 0;
    z->itf = ' ';
    z->destIdErrCnt = 0;
    z->destIdDiff = 0;
    z->phyRxFrameCnt = 0;
    z->frameDiff = 0;
}

static void updateStateInfo()
{   
    uint8 zingState = getZingState();
    setZingState(zingState,0xE4,getI2CReadBuffer()+ICD_RX_MODEM_STATE_OFFSET);     //수신기 모뎀 상태 0x00 : 정상, 0xE4 : 무선영상 수신기 모뎀 이상
    if(zingState==1) clearZcdZingInfo(getZcdFrame());
    
    setImuState(getImuState(),0xE6,getI2CReadBuffer()+ICD_RX_IMU_STATE_OFFSET);         //수신기 IMU 상태 0x00: 정상, 0xE6: 무선영상 수신기 IMU 이상    
    setPairingState(getSystemMode(), getI2CReadBuffer()+ICD_BLE_OFFSET);                //BLE state
}

static uint16 timerCount = 0;
static uint16 btnHoldCount = 0;

void TimerCallback(void)
{
    timerCount++;
    
    if(timerCount==1000) {  //1 second
        updateStateInfo();
        
        //LED_RED_Write(!LED_RED_Read());
        timerCount = 0;
        btnHoldCount++;
    }
}

static void InitOnBleDisconnect()
{
    if(cyBle_state!=CYBLE_STATE_CONNECTED) {
        memset(getI2CReadBuffer()+ICD_SCOPE_VIDEO_KIND_OFFSET,0xFF,ICD_TX_MODEM_STATE_OFFSET-ICD_SCOPE_VIDEO_KIND_OFFSET+1);
        getI2CReadBuffer()[ICD_TX_IMU_STATE_OFFSET] = 0xFF;
    }
}

int main(void)
{
    PW_EN_Write(0);
    HGATE_Con1_Write(0x00);
    HGATE_Con2_1_Write(0x00);
    HGATE_Con2_2_Write(0x00);
    CyDelay(1000);
    
    PW_EN_Write(1);
    CyDelay(500);
    HGATE_Con1_Write(0x04);
    HGATE_Con2_1_Write(0x03&(0x04));
    HGATE_Con2_2_Write((0x0C&0x04)>>2);
    HGATE_Con1_Write(HGATE_Con1_VAL);
    HGATE_Con2_1_Write(0x03&(HGATE_Con2_VAL));
    HGATE_Con2_2_Write((0x0C&HGATE_Con2_VAL)>>2);
    SPDT_Write(0);

    setBccVersion();
    ivfCom.wirelessVideoChannelMode = 0x1;  //자동 채널 설정 모드를 default 초기값으로 지정
    
    CySysTickStart();
    /* Find unused callback slot and assign the callback. */
    for (uint8 i = 0u; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            /* Set callback */
            CySysTickSetCallback(i, TimerCallback);
            break;
        }
    }
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    i2cs_start();

    const FlashData_t* fd = LoadStoredPeripheralAddress();	//Load stored address from flash
    for(int i=0;i<5;i++) {
        if(fd) {
            //LED_GREEN_Write(!LED_GREEN_Read()); //loaded from flash
        }else{
            //LED_RED_Write(!LED_RED_Read());     //could not load from empty flash
        }
        CyDelay(100);
    }

    CyBle_Start( CyBle_AppCallback );
    
    UART_IMU_StartAndInitialize();
    
    for(;;)
    {          
        InitOnBleDisconnect();
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process(getZcdFrame());
        imu_process_uart_data(onImuFrame);
        processImuCommand(ivfCom.wirelssVideoReceiverImuOutputType,ivfCom.wirelessVideoReceiverImuCalibrate);
        if( (ivfCom.wirelessVideoReceiverOperationModeStatus==0x0) || (ivfCom.wirelessVideoReceiverOperationModeStatus==0x1)) {
            //무선영상 수신기 운용모드가 default또는 운용모드인 경우만 출력 변경 함수 호출 허용
            processPower(ivfCom.wirelessVideoReceiverPower);
        }
    }
}
