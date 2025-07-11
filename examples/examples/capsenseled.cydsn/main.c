#include <project.h>
#include <stdio.h>

static char msg[128];

uint16 fingerPos    = 0xFFFF;
uint16 fingerPosOld = 0xFFFF;

int capsenseNotify;

/***************************************************************
 * Function to update the LED state in the GATT database
 **************************************************************/
void updateLed()
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T 	tempHandle;
   
    uint8 red_State = !red_Read();
    
    if(CyBle_GetState() != CYBLE_STATE_CONNECTED)
        return;
    
    tempHandle.attrHandle = CYBLE_LEDCAPSENSE_LED_CHAR_HANDLE;
  	tempHandle.value.val = (uint8 *) &red_State;
    tempHandle.value.len = 1;
    CyBle_GattsWriteAttributeValue(&tempHandle,0,&cyBle_connHandle,CYBLE_GATT_DB_LOCALLY_INITIATED);  
    
    sprintf(msg,"updateLed, CyBle_GattsWriteAttributeValue, red state=%d\r\n",red_State);
    UART_UartPutString(msg);
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
        CyBle_GattsNotification(cyBle_connHandle,&tempHandle);
        sprintf(msg,"updateCapsense, CyBle_GattsNotification, pos=%d\r\n",fingerPos);
        UART_UartPutString(msg);
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
        UART_UartPutString("CYBLE_EVT_STACK_ON\r\n");
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            capsenseNotify = 0;
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            pwm_Start();
            UART_UartPutString("Advertising fast\r\n");
            break;
            
        case CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP:
            UART_UartPutString("CYBLE_EVT_GAPP_ADVERTISEMENT_START_STOP\r\n");
            break;
        
        /* when a connection is made, update the LED and Capsense states in the GATT database and stop blinking the LED */    
        case CYBLE_EVT_GATT_CONNECT_IND:
            UART_UartPutString("CYBLE_EVT_GATT_CONNECT_IND\r\n");
            updateLed();
            updateCapsense();  
            pwm_Stop();
		    break;
            
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            UART_UartPutString("CYBLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            break;

        /* handle a write request */
        case CYBLE_EVT_GATTS_WRITE_REQ:
            UART_UartPutString("CYBLE_EVT_GATTS_WRITE_REQ");
            wrReqParam = (CYBLE_GATTS_WRITE_REQ_PARAM_T *) eventParam;
			      
            /* request write the LED value */
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_LED_CHAR_HANDLE)
            {
                UART_UartPutString(",request write LED value");
                
                /* only update the value and write the response if the requested write is allowed */
                if(CYBLE_GATT_ERR_NONE == CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED))
                {
                    red_Write(!wrReqParam->handleValPair.value.val[0]);
                    CyBle_GattsWriteRsp(cyBle_connHandle);
                    UART_UartPutString(",CyBle_GattsWriteRsp");
                }
            }
            
            /* request to update the CapSense notification */
            if(wrReqParam->handleValPair.attrHandle == CYBLE_LEDCAPSENSE_CAPSENSE_CAPSENSECCCD_DESC_HANDLE) 
            {
                UART_UartPutString(",request update CapSense notification");
                
                CyBle_GattsWriteAttributeValue(&wrReqParam->handleValPair, 0, &cyBle_connHandle, CYBLE_GATT_DB_PEER_INITIATED);
                capsenseNotify = wrReqParam->handleValPair.value.val[0] & 0x01;
                CyBle_GattsWriteRsp(cyBle_connHandle);
                
                sprintf(msg,",capsenseNotity=%d",capsenseNotify);
                UART_UartPutString(msg);
            }
            UART_UartPutString("\r\n");
			break;  
            
        case CYBLE_EVT_GATT_DISCONNECT_IND:
            UART_UartPutString("CYBLE_EVT_GATT_DISCONNECT_IND\r\n");
            break;
        
        default:
            sprintf(msg,"unhandled BLE event = 0x%lx\r\n",event);
            UART_UartPutString(msg);
            break;
    }
} 

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
 
    UART_Start();
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
        }
   
        CyBle_ProcessEvents();
        CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);    
    }
}
