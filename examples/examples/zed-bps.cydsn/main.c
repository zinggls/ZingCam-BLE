#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>
#include <ZedFrame.h>

#define ASCII_LF '\n'

uint16 fingerPos    = 0xFFFF;
uint16 fingerPosOld = 0xFFFF;
uint8_t isStackBusy = 0;

ulong notifyCustom = 0;
ulong writereqCustom = 0;

ZED_FRAME data = {
    .usb = 0,
    .bnd = 0,
    .ppid = 0,
    .devid = 0,
    .trt = 0,
    .ack = 0,
    .ppc = 0,
    .txid = 0,
    .rxid = 0,
    .run = 0,
    .cnt = 0,
    .pos = 0
};

typedef struct {
    int usb;            // USB value, e.g., "2"
    char bnd;           // BND value, e.g., "L"
    unsigned int ppid;  // PPID value, e.g.,"0xABCD"
    unsigned int devid; // DeviceID, e.g.,  "0x3500"
    char trt;           // TRT value, e.g., "B"
    char ack;           // ACK value, e.g., "N"
    char ppc;           // PPC value, e.g., "P"
    unsigned int txid;  // TXID value, e.g.,"0x0"
    unsigned int rxid;  // RXID value, e.g.,"0x0"
    char run;           // RUN value, e.g., "N"
    unsigned int cnt;   // CNT value, e.g., "0"
} ZING_Data;

static UartBuf uBuf;    //Circular buffer for UART data
static ZING_Data zing_data;

CY_ISR(UART_ZING_RX_INTERRUPT)
{
    char ch = UART_ZING_GetChar();
    if (ch != 0) {
        UartBuf_write_char(&uBuf,ch);  // Write character to circular buffer

        // Check for end of message
        if (ch == ASCII_LF) {
            uBuf.message_complete = true;
        }
    }

    // Clear the interrupt to prevent retriggering
    UART_ZING_RX_ClearInterrupt();
}

static void PrintZingData(ZING_Data* z) {
    L("ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d\r\n",
        z->usb, z->bnd, z->ppid, z->devid, z->trt, z->ack, z->ppc, z->txid, z->rxid, z->run, z->cnt);
}

// Function to process data when a complete message is available
static void process_uart_data()
{
    if (uBuf.message_complete) {
        // Extract complete message from buffer
        char zing_status[MAX_BUFFER_LENGTH] = {0};
        uint16_t cnt = 0;
        
        while (!UartBuf_is_empty(&uBuf)) {
            zing_status[cnt++] = UartBuf_read_char(&uBuf);
        }
        
        zing_status[cnt] = '\0';  // Null-terminate the string
        uBuf.message_complete = false;

        // Parsing the values into the structure
        if (sscanf(zing_status, 
                   "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                   &zing_data.usb,
                   &zing_data.bnd,
                   &zing_data.ppid,
                   &zing_data.devid,
                   &zing_data.trt,
                   &zing_data.ack,
                   &zing_data.ppc,
                   &zing_data.txid,
                   &zing_data.rxid,
                   &zing_data.run,
                   &zing_data.cnt) != 11) {
//            UART_DBG_UartPutString("Parsing Error\r\n");
//            UART_DBG_UartPutString("Received: ");
//            UART_DBG_UartPutString(zing_status);
//            UART_DBG_UartPutString("\r\n");
        } else {
//            PrintZingData(&zing_data);
            data.usb = zing_data.usb;
            data.bnd = zing_data.bnd;
            data.ppid = zing_data.ppid;
            data.devid = zing_data.devid;
            data.trt = zing_data.trt;
            data.ack = zing_data.ack;
            data.ppc = zing_data.ppc;
            data.txid = zing_data.txid;
            data.rxid = zing_data.rxid;
            data.run = zing_data.run;
            data.cnt = zing_data.cnt;
            data.pos = 0;
        }
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
#if _VERBOSE
            UART_UartPutString("CYBLE_EVT_GATTS_WRITE_REQ");
#endif
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
            
            if (wrReqParam->handleValPair.attrHandle == CYBLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE) {
                if (wrReqParam->handleValPair.value.len == sizeof(ZED_FRAME)) {
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                
                    writereqCustom++;
#if _VERBOSE    
                    MyData* receivedData = (MyData*)wrReqParam->handleValPair.value.val;
                    uint8_t command = receivedData->val1;
                    sprintf(msg,",custom request from the central, command=%d",command);
                    UART_UartPutString(msg);
#endif
                }
            }
#if _VERBOSE
            UART_UartPutString("\r\n");
#endif
			break;  
            
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            L("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            break;
            
        case CYBLE_EVT_STACK_BUSY_STATUS:
            isStackBusy = *(uint8_t *)eventParam;
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

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
 
    UART_DBG_Start();
    UART_ZING_Start();
    UartBuf_init(&uBuf);
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    capsense_Start();
    capsense_InitializeEnabledBaselines();
    
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);

    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
    myDataHandle.value.val = (uint8_t*)&data;
    myDataHandle.value.len = sizeof(ZED_FRAME);
    
    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        if(!capsense_IsBusy())
        {
            fingerPos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
        }
        
        data.pos = fingerPos;
        CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
        
        CYBLE_API_RESULT_T res = CyBle_GattsNotification(cyBle_connHandle,&myDataHandle);
        if(res==CYBLE_ERROR_OK) notifyCustom++;

#ifndef _VERBOSE
        L("[ble-perSvr] cyBle_state:0x%x OUT:Notify{ Custom=%lu }    IN:WriteReq{ Custom=%lu }\r\n",
            cyBle_state,notifyCustom,writereqCustom);
#endif
   
        CyBle_ProcessEvents();
        process_uart_data();
    }
}
