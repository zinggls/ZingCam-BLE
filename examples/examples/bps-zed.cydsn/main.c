#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>

#define ASCII_LF '\n'

uint16 fingerPos    = 0xFFFF;
uint16 fingerPosOld = 0xFFFF;
uint8_t isStackBusy = 0;

ulong notifyCustom = 0;
ulong notifyCapsense = 0;
ulong writereqCustom = 0;
ulong writereqCapsense = 0;

int capsenseNotify;

typedef struct
{
    uint8_t val1;
    uint8_t val2;    
    uint16_t values[3];
} MyData;

MyData data = {
    .val1 = 10,        // Assign an appropriate value for val1 (e.g., 10)
    .val2 = 20,        // Assign an appropriate value for val2 (e.g., 20)
    .values = {300, 400, 500}  // Initialize the values array with specific values
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
            UART_DBG_UartPutString("Parsing Error\r\n");
            UART_DBG_UartPutString("Received: ");
            UART_DBG_UartPutString(zing_status);
            UART_DBG_UartPutString("\r\n");
        } else {
            PrintZingData(&zing_data);
        }
    }
}

/***************************************************************
 * Function to update the CapSesnse state in the GATT database
 **************************************************************/
void updateCapsense()
{
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;
    
	CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
    
    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_CAPSENSE_CHAR_HANDLE;
  	tempHandle.value.val = (uint8 *)&fingerPos;
    tempHandle.value.len = 2; 
    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED );  
    
    /* send notification to client if notifications are enabled and finger location has changed */
    if (capsenseNotify && (fingerPos != fingerPosOld) ) {
        CYBLE_API_RESULT_T res = CyBle_GattsNotification(cyBle_connHandle,&tempHandle);
        if(res==CYBLE_ERROR_OK) {
            notifyCapsense++;
#if _VERBOSE            
            sprintf(msg,"updateCapsense, CyBle_GattsNotification, pos=0x%x\r\n",fingerPos);
            UART_UartPutString(msg);
#endif
        }
    }
    
    fingerPosOld = fingerPos;
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
            capsenseNotify = 0;
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            L("Advertising fast\r\n");
            break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            L("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            break;
        
        /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */    
        case CYBLE_EVT_GATT_CONNECT_IND:
            L("CYBLE_EVT_GATT_CONNECT_IND\r\n");
            updateCapsense();  
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
                if (wrReqParam->handleValPair.value.len == sizeof(MyData)) {
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
            
            /* request to update the CapSense notification */
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_CAPSENSE_CAPSENSECCCD_DESC_HANDLE) 
            {
#if _VERBOSE
                UART_UartPutString(",request update CapSense notification");
#endif
                
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                capsenseNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
                
                writereqCapsense++;
#if _VERBOSE
                sprintf(msg,",capsenseNotity=%d",capsenseNotify);
                UART_UartPutString(msg);
#endif
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

    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        if(!capsense_IsBusy())
        {
            fingerPos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
            updateCapsense();
            
            if(isStackBusy==CYBLE_STACK_STATE_FREE) {
                CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
                myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_HANDLE;
                data.val1 = fingerPos;
                myDataHandle.value.val = (uint8_t*)&data;
                myDataHandle.value.len = sizeof(MyData);
                CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
                
                if (capsenseNotify) {
                    CYBLE_API_RESULT_T res = CyBle_GattsNotification(cyBle_connHandle,&myDataHandle);
                    if(res==CYBLE_ERROR_OK) notifyCustom++;
                }
            }
#ifndef _VERBOSE
//            L("[ble-perSvr] cyBle_state:0x%x OUT:Notify{ Custom=%lu, Capsense=%lu }    IN:WriteReq{ Custom=%lu, Capsense=%lu }\r\n",
//                cyBle_state,notifyCustom,notifyCapsense,writereqCustom,writereqCapsense);
#endif
        }
   
        CyBle_ProcessEvents();
        process_uart_data();
    }
}
