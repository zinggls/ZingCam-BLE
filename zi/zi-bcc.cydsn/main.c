
#include "project.h"
#include <stdio.h>
#include <ZFrame.h>
#include <UartBuf.h>
#include <Zing.h>
#include <i2cs.h>
#include "imu.h"
#if 0
#include "NoLog.h"
#else
#include "Log.h"
#endif
#include "bcc.h"
#include "icd.h"
#include "Peripheral.h"
#include "git_describe.h"
#include "versionInfo.h"
#include "FlashRow.h"

#define BUTTON_HOLD_TIME_SEC  3   //3 seconds
#define ITF_CRITERIA        10

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
        
        if(bITF == false && interferenceCount>=ITF_CRITERIA) {
            //Interference appeared
            if(ivfCom.wirelessVideoChannelMode==0x00 || ivfCom.wirelessVideoChannelMode==0x01 ) { //자동 채널 설정 모드
                sendITF('Y');
                SPDT_Write(~SPDT_Read());
            }
            bITF = true;
        } else if(bITF==false && interferenceCount < ITF_CRITERIA) {
            //Negligible interference (DO NOTHING)
        } else if(bITF==true && interferenceCount >= ITF_CRITERIA) {
            //Still under interference (DO NOTHING)
        } else if(bITF==true && interferenceCount < ITF_CRITERIA) {
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
        
        CyDelay(1000);
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
        if(bProcessed) CyDelay(1000);
    }
}

static void processImuCommand(uint8_t imuOutputType,uint8_t imuCalibrate)
{
    processImuOutputType(imuOutputType);
    processImuCalibrate(imuCalibrate);
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
/*
CY_ISR( Pin_SW2_Handler )
{   
    //LED_RED_Write( ~LED_RED_Read() );
    
    if(Pin_SW2_Read()==0) { // Button pressed
        btnHoldCount = 0;
    }else{ // Button released
        L("Button hold time %d second(s)\r\n",btnHoldCount);
        if(btnHoldCount>BUTTON_HOLD_TIME_SEC) {
            LED_GREEN_Write( 0 );
            CyDelay(500);
            LED_GREEN_Write( 1 );
            
            cystatus status;
            if(ClearPeripheralAddress(&status)) {
                LED_BLUE_Write( 0 );
                L("Address in flash cleared\r\n");
                CyDelay(500);
                CySoftwareReset();
            }else{
                 L("Clearing Address in flash failed(0x%02X)\r\n",status);
            }
        }
    }
    
    Pin_SW2_ClearInterrupt();
}
*/
void HGATE_CON2_Write(int val)
{
    HGATE_Con2_1_Write(0x03&(val));
    HGATE_Con2_2_Write((0x0C&val)>>2);
}
void HGate_init()
{
        HGATE_Con1_Write(0x02);  //
        HGATE_CON2_Write(0x02); //-1V
        /***********************************
         Gate Voltage value
        
        0x00 = -3.119V
        0x04 = -1.557V
        0x01 = -1.010V   ----- Default
        0x08 = -0.7773V
        0x02 = -0.5030V
        0x0C = -0.3872V
        0x03 = -0.2500V
        0x07 = -0.1234V
        0x0B = -0.0601V
        0x0F = -0.0284V
        
        **************************************/
}

int main(void)
{
    UART_DBG_PutString("zi-bcc main\r\n");
    
    PW_EN_Write(0);
    HGATE_Con1_Write(0x00);
    HGATE_Con2_1_Write(0x00);
    HGATE_Con2_2_Write(0x00);
    CyDelay(1000);
    
    PW_EN_Write(1);
    CyDelay(2000);
    HGATE_Con1_Write(0x03);
    HGATE_Con2_1_Write(0x03);
    HGATE_Con2_2_Write(0x00);
    SPDT_Write(0);
    CyDelay(10);

    setBccVersion();
    
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
#if 0
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
#endif
    UART_DBG_Start();
    i2cs_start();

    const FlashData_t* fd = LoadStoredPeripheralAddress();	//Load stored address from flash
    for(int i=0;i<5;i++) {
        if(fd) {
//            LED_GREEN_Write(!LED_GREEN_Read()); //loaded from flash
        }else{
            //LED_RED_Write(!LED_RED_Read());     //could not load from empty flash
        }
        CyDelay(100);
    }

    CyBle_Start( CyBle_AppCallback );
//    Pin_SW2_Int_StartEx( Pin_SW2_Handler );
    
    CyDelay(1000);
    UART_IMU_StartAndInitialize();
    
    for(;;)
    {          
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process(getZcdFrame());
        imu_process_uart_data(onImuFrame);
        processImuCommand(ivfCom.wirelssVideoReceiverImuOutputType,ivfCom.wirelessVideoReceiverImuCalibrate);
    }
}
