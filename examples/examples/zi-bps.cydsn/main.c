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
    ZXX_FRAME *z = &zxxFrame;
    if(zxxKind==ZED) L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, ZED USB:%d CNT:%d\r\n", GIT_INFO,cyBle_state,notifyCustom,z->pos,getWritereqCustom(),z->usb,z->cnt);
    if(zxxKind==ZCH) L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, ZCH USB:%d CNT:%d\r\n", GIT_INFO,cyBle_state,notifyCustom,z->pos,getWritereqCustom(),z->usb,z->cnt);
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

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
    
    LED_GREEN_Write(1);

    UART_DBG_Start();
    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    capsense_Start();
    capsense_InitializeEnabledBaselines();
    
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    UART_IMU_StartAndInitialize();

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
        
        uint8_t *frame = (uint8_t *)&zxxFrame;
        if(isNoZingCb(count,200,&ZingCbCount)) memset(frame,0,getFrameSize());  //reset on no zing data
        
        myDataHandle.value.val = frame;
        myDataHandle.value.len = getFrameSize();
        CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
        if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
        
        zxxLog();
   
        CyBle_ProcessEvents();
        zing_process_uart_data();
        imu_process_uart_data(onImuFrame);
        
        count++;
    }
}
