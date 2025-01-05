
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

CY_ISR(TimerCallback)
{
    timerCount++;
    
    if(timerCount==1000) {  //1 second
        updateStateInfo();
        
        LED_RED_Write(!LED_RED_Read());
        timerCount = 0;
    }
}

int main(void)
{
    Timer_Start();
    timer_isr_StartEx(TimerCallback);
    
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    i2cs_start();
    CyBle_Start( CyBle_AppCallback );
    
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
