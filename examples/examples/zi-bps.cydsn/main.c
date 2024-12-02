#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <ZingUart.h>
#include "ZFrame.h"
#include "imu.h"

static uint16 notifyCustom = 0;
static uint16 writereqCustom = 0;

static ZED_FRAME zedFrame;
static ZCH_FRAME zchFrame;
extern ZingRxCallback zingRxCb;

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    if(*getZxxKind()==Unknown) *getZxxKind() = detectZxx(buf);
    if(*getZxxKind()!=ZED && *getZxxKind()!=ZCH) return;

    // Parsing the values into the structure
    void *frame = getFrame(&zedFrame,&zchFrame);
    CYASSERT(frame);
    if (!parse(*getZxxKind(),frame,buf)) {
#ifdef ZXX_DEBUG
        UART_DBG_UartPutString("Parsing Error\r\n");
        UART_DBG_UartPutString("Received: ");
        UART_DBG_UartPutString(zing_status);
        UART_DBG_UartPutString("\r\n");
#endif
    }
}

/***************************************************************
 * Function to handle the BLE stack
 **************************************************************/
void BleCallBack(uint32 event, void* eventParam)
{
    CYBLE_GATTS_WRITE_REQ_PARAM_T *wrReqParam;

    switch(event)
    {
        /* if there is a disconnect or the stack just turned on from a reset then start the advertising and turn on the LED blinking */
        case CYBLE_EVT_STACK_ON:
        L("CYBLE_EVT_STACK_ON\r\n");
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            L("Advertising fast\r\n");
            break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            L("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            break;
        
        /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */    
        case CYBLE_EVT_GATT_CONNECT_IND:
            L("CYBLE_EVT_GATT_CONNECT_IND\r\n");
		    break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            L("CYBLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            break;

        /* handle a write request */
        case CYBLE_EVT_GATTS_WRITE_REQ:
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
            if (wrReqParam->handleValPair.attrHandle == CYBLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE) {
                if (wrReqParam->handleValPair.value.len == sizeof(ZED_FRAME)) {
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                
                    writereqCustom++;
                }
            }
			break;  
            
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            L("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            break;
            
        case CYBLE_EVT_STACK_BUSY_STATUS:
            break;
            
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            {
                // Respond to MTU exchange request from Central
                CYBLE_GATT_XCHG_MTU_PARAM_T mtuExchgParam = *(CYBLE_GATT_XCHG_MTU_PARAM_T*)eventParam;
                CYBLE_API_RESULT_T res = CyBle_GattsExchangeMtuRsp(cyBle_connHandle, mtuExchgParam.mtu);
                L("MTU Exchange Request Handled, MTU size: %d, CyBle_GattsExchangeMtuRsp=0x%x\r\n", mtuExchgParam.mtu,res);
            }
            break;
            
        case CYBLE_EVT_GAP_DATA_LENGTH_CHANGE:
            // Data length parameters changed
            {
                CYBLE_GAP_DATA_LENGTH_T* dataLength = (CYBLE_GAP_DATA_LENGTH_T*)eventParam;

                // Log the updated parameters
                L("Data Length Change Event:\n");
                L("  Max TX Octets: %d\n", dataLength->maxTxOctets);
                L("  Max RX Octets: %d\n", dataLength->maxRxOctets);
                L("  Max TX Time: %d us\n", dataLength->maxTxTime);
                L("  Max RX Time: %d us\n", dataLength->maxRxTime);
            }
            break;
            
        default:
            L("unhandled BLE event = 0x%lx\r\n",event);
            break;
    }
} 

static void zxxLog()
{
    if(*getZxxKind()==ZED) {
        ZED_FRAME *z = &zedFrame;
        L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, ZED USB:%d CNT:%d\r\n", GIT_INFO,cyBle_state,notifyCustom,z->pos,writereqCustom,z->usb,z->cnt);
    }
    
    if(*getZxxKind()==ZCH) {
        ZCH_FRAME *z = &zchFrame;
        L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, ZCH USB:%d CNT:%d\r\n", GIT_INFO,cyBle_state,notifyCustom,z->pos,writereqCustom,z->usb,z->cnt);
    }
}

static void onImuFrame(const ImuFrame *imu)
{

}

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 

    UART_DBG_Start();
    ZingUart_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    capsense_Start();
    capsense_InitializeEnabledBaselines();
    
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    UART_IMU_StartAndInitialize();

    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
    
    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        if(!capsense_IsBusy())
        {
            if(*getZxxKind()==ZED) zedFrame.pos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            if(*getZxxKind()==ZCH) zchFrame.pos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
        }
        
        uint8_t *frame = (uint8_t *)getFrame(&zedFrame,&zchFrame);
        if(frame) {
            myDataHandle.value.val = frame;
            myDataHandle.value.len = getFrameSize();
            CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
            if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
        }
        
        zxxLog();
   
        CyBle_ProcessEvents();
        zing_process_uart_data();
        imu_process_uart_data(onImuFrame);
    }
}
