
#include "project.h"
#include <stdio.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <UartBuf.h>
#include <Zing.h>
#include <i2cs.h>
#include "imu.h"
#include "NoLog.h"
#include "bcc.h"
#include "icd.h"

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

static void onImuFrame(const ImuFrame *imu)
{
    memcpy(getI2CReadBuffer()+IMU_RX_OFFSET,imu->data,IMU_FRAME_SIZE); //무선영상 수신기IMU 34
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
    UART_IMU_StartAndInitialize();
    
    for(;;)
    {          
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process(getZcdFrame());
        imu_process_uart_data(onImuFrame);
    }
}
