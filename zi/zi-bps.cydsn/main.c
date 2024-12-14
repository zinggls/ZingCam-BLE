#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <Zing.h>
#include "ZFrame.h"
#include "imu.h"
#include "bps.h"
#include "led.h"
#include "icd.h"
#include "NoLog.h"

static uint16 notifyCustom = 0;

static ZXX_FRAME zxxFrame;
static uint32 ZingCbCount = 0;

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    if(zxxKind==Unknown) zxxKind = detectZxx(buf);
    if(zxxKind!=ZED && zxxKind!=ZCH) return;

    // Parsing the values into the structure
    void *frame = &zxxFrame;
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
    const char *name = (zxxFrame.kind==ZCH)?"ZCH":"ZED";
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

static short toShort(const char *data)
{
    return (short)((data[1] << 8) | data[0]);
}

static void onImuFrame(const ImuFrame *imu)
{
    ZXX_FRAME *z = &zxxFrame;
    z->imu1 = toShort(imu->data);
    z->imu2 = toShort(imu->data+2);
    z->imu3 = toShort(imu->data+4);
    z->imu4 = toShort(imu->data+6);
    z->imu5 = toShort(imu->data+8);
    z->imuChecksum = toShort(imu->data+10);
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

static void updateStateInfo()
{
    zxxFrame.txStateBattery = 78;   //TO DO 배터리 용량 파악하는 기능 구현전까지 하드코딩 상태로 둔다 (78%로 표시됨)
    
    uint8 zingState = getZingState();
    setZingState(zingState,0xE3,(uint8*)&zxxFrame.txStateModem);     //수신기 모뎀 상태 0x00 : 정상, 0xE3 : 무선영상 송신기 모뎀 이상
    if(zingState==1) clearZxxZingInfo(&zxxFrame);
    
    setImuState(getImuState(),0xE5,(uint8*)&zxxFrame.txStateIMU);         //송신기 IMU 상태 0x00: 정상, 0xE5: 무선영상 수신기 IMU 이상
}

CY_ISR(Timer_ISR)
{
    // Clear the interrupt
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);

    // Add your custom code here
    updateStateInfo();
    
    LED_RED_Write(!LED_RED_Read());
}

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
    
    setGreen(LED_OFF);

    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    capsense_Start();
    capsense_InitializeEnabledBaselines();
    
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    UART_IMU_StartAndInitialize();
    
    Timer_Start();
    Timer_ISR_StartEx(Timer_ISR);

    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE;
    
    uint32 count = 0;
    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        if(!capsense_IsBusy())
        {
            zxxFrame.pos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
        }
        
        myDataHandle.value.val = (uint8_t *)&zxxFrame;
        myDataHandle.value.len = sizeof(ZXX_FRAME);
        CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
        if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
        
        zxxLog();
   
        CyBle_ProcessEvents();
        zing_process_uart_data();
        imu_process_uart_data(onImuFrame);
        
        count++;
    }
}
