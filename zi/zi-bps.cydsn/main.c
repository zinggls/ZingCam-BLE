#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>
#include <ZFrame.h>
#include <Zing.h>
#include "Peripheral.h"
#include "imu.h"
#include "bps.h"
#include "led.h"
#include "icd.h"
#include "NoLog.h"
#include "i2cs.h"
#include "git_describe.h"
#include "versionInfo.h"
#include "hgate.h"

#define BATTERY_FULL_CHARGE 1658
#define BATTERY_NEED_CHARGE 1568

static uint16 notifyCustom = 0;
static uint32 ZingCbCount = 0;

static void setBbsVersion()
{
    memset(&peripheral.bpsVer,0,sizeof(Version));
    strcpy(peripheral.bpsVer.info,GIT_DESCRIBE);
}

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    if(zxxKind==Unknown) zxxKind = detectZxx(buf);
    if(zxxKind!=ZED && zxxKind!=ZCH) return;

    // Parsing the values into the structure
    void *frame = &peripheral.zxxFrame;
    CYASSERT(frame);
    if (!parse(frame,buf)) {
#ifdef ZXX_DEBUG
        L("Parsing Error\r\n");
        L("Received: ");
        L(zing_status);
        L("\r\n");
#endif
    }else{
        ZingCbCount++;
    }
}

static void zxxLog()
{
    const char *name = (peripheral.zxxFrame.kind==ZCH)?"ZCH":"ZED";
    L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, %s USB:%d CNT:%d IvfCom{%d %d %d %d %d %d %d %d %d %d %d}\r\n", 
        GIT_INFO,cyBle_state,notifyCustom,zxxFrame.pos,getWritereqCustom(),name,zxxFrame.usb,zxxFrame.cnt,
        ivfCom.scopeCamera,
        ivfCom.scopeOutput,
        ivfCom.wirelessVideoChannelMode,
        ivfCom.wirelessVideoChannelInformation,
        ivfCom.scopeOperationMode,
        ivfCom.wirelessVideoTransmitterOperationModeStatus,
        ivfCom.wirelessVideoReceiverOperationModeStatus,
        ivfCom.wirelessVideoTransmitterImuOutputType,
        ivfCom.wirelessVideoTransmitterImuCalibrate,
        ivfCom.wirelssVideoReceiverImuOutputType,
        ivfCom.wirelessVideoReceiverImuCalibrate);
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
    IMU *i = &peripheral.imu;
    
    if(sof==1) {    //Euler
        i->imu1 = 0x0000;
        i->imu2 = toShort(imu->data+2);
        i->imu3 = toShort(imu->data+4);
        i->imu4 = toShort(imu->data+6);
        i->imu5 = toShort(imu->data+8);
        i->imuChecksum = toShort(imu->data+10);
    }
    
    if(sof==2) {    //Quaternion
        i->imu1 = toShort(imu->data+2);
        i->imu2 = toShort(imu->data+4);
        i->imu3 = toShort(imu->data+6);
        i->imu4 = toShort(imu->data+8);
        i->imu5 = toShort(imu->data+10);
        i->imuChecksum = toShort(imu->data+12);
    }
}

static void clearZxxZingInfo(ZXX_FRAME *z)
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
}

static int16_t adc_measure(uint8_t channel)
{
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    return ADC_GetResult16(channel);
}

static void display_battery_level(int percentage)
{   
    if(percentage>=66) {
        Batt_state_LED_Write(0x6);
    }else if(percentage<66 && percentage>=33){
        Batt_state_LED_Write(0x5);
    }else if(percentage<33 && percentage>=10){
        Batt_state_LED_Write(0x0);
    }else{
        Batt_state_LED_Write(~Batt_state_LED_Read());
    }
}

static int adc_percentage(int16_t val)
{    
    float fPer = ((float)val/(float)BATTERY_FULL_CHARGE)*100.0;
    return (int)fPer;
}

static void updateStateInfo()
{
    int percentage = adc_percentage(adc_measure(0));
    peripheral.txState.txStateBattery = (char)percentage;
    display_battery_level(percentage);
    
    uint8 zingState = getZingState();
    setZingState(zingState,0xE3,(uint8*)&peripheral.txState.txStateModem);     //수신기 모뎀 상태 0x00 : 정상, 0xE3 : 무선영상 송신기 모뎀 이상
    if(zingState==1) clearZxxZingInfo(&peripheral.zxxFrame);
    
    setImuState(getImuState(),0xE5,(uint8*)&peripheral.txState.txStateIMU);    //송신기 IMU 상태 0x00: 정상, 0xE5: 무선영상 수신기 IMU 이상
}

void resetScopeStates()
{
    peripheral.scope.scopeStateKind = 0x0;
    peripheral.scope.scopeStateOut = 0xff;
    peripheral.scope.scopeStateBattery = 0xff;
    peripheral.scope.scopeStateIR = 0xff;
    peripheral.scope.scopeStateEO = 0xff;
    peripheral.scope.scopeDetect = 0x0;
}

static uint16 timerCount = 0;
static uint16 scopeMonTimer = 0;
static uint8 changeCount = 0;
static uint8 prevScopeWorkingState = 0;

void TimerCallback(void)
{
    timerCount++;
    scopeMonTimer++;
    
    if(timerCount==1000) {  //1 second
        updateStateInfo();
        
        //LED_RED_Write(!LED_RED_Read());
        timerCount = 0;        
        
        if(cyBle_state==2) {// BT connected
            if(peripheral.zxxFrame.run=='Y') {
                TX_Run_Write(~TX_Run_Read());  //Flickering on running
            }else{
                TX_Run_Write(0x0);  //LED ON
            }
        }else{
            TX_Run_Write(0x1);  //LED OFF
        }
    }
    
    if(prevScopeWorkingState!=scopeWorkingState) {
        changeCount++;
        prevScopeWorkingState = scopeWorkingState;
    }
    
    if(scopeMonTimer>=2000) {   //2.0 second
        if(changeCount==0) {
            resetScopeStates();
            i2c_command_to_bcc();
        }

        //reset timer and count
        scopeMonTimer = 0;
        changeCount = 0;
    }
}

CY_ISR( SW_PW_Handler )
{   
    UART_ZING_RX_INTR_Disable();
    SW_PW_Int_Disable();
    CyDelay(10);
    u_int8_t SWP=SW_PW_Read();
    u_int8_t SWL=SW_LED_Read();
    if(((SWP==1)&&(SWL==0)) || ((SWP==0)&&(SWL==1)))
    {
        Reset_Write(1);
        HGATE_Con1_Write(0x0);
        PW_EN_Write(1);
        UART_ZING_PutString("POWER On\n");
    }
    else if((SWP==0)&&(SWL==0))
    {
        HGATE_Con1_Write(0x0);
        PW_EN_Write(0);
        UART_ZING_PutString("POWER OFF\n");
    }
    UART_ZING_RX_INTR_Enable();
    SW_PW_Int_Enable();
    SW_PW_ClearInterrupt();
}

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    PW_EN_Write(0);
    HGATE_Con1_Write(0x00);
    PW_EN_Write(1);
    CyDelay(500);
    HGATE_Con1_Write(0x04);
    HGATE_Con1_Write(HGATE_Con1_VAL);
#ifdef WITHOUT_CB
    PW_EN_Write(1);
    CyDelay(2000);
    HGATE_Con1_Write(0x03);
      
    CyDelay(10);
#endif
    
    CyGlobalIntEnable; 
    
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
    
    setBbsVersion();
//    setGreen(LED_OFF);
    CH_LED_Write(0x3);  //LED Off

    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    
    i2cs_start();
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    CyDelay(1000);
    UART_IMU_StartAndInitialize();
    ADC_Start();

    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE;
#ifndef WITHOUT_CB
    SW_PW_Int_StartEx( SW_PW_Handler );
#endif
    uint32 count = 0;
    for(;;)
    {        
        myDataHandle.value.val = (uint8_t *)&peripheral;
        myDataHandle.value.len = sizeof(PERIPHERAL);
        CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
        if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
        
        zxxLog();
   
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process();
        imu_process_uart_data(onImuFrame);
        processImuCommand(ivfCom.wirelessVideoTransmitterImuOutputType,ivfCom.wirelessVideoTransmitterImuCalibrate);
        
        if(peripheral.zxxFrame.bnd=='H') {
            CH_LED_Write(0x1);  //LED Red
        }else if(peripheral.zxxFrame.bnd=='L') {
            CH_LED_Write(0x2);  //LED Green
        }else{
            CH_LED_Write(0x3);  //LED Off
        }
        
        count++;
    }
}
