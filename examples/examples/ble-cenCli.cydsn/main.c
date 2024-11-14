
#include "project.h"
#include <stdio.h>

// Modes for a statemachine
typedef enum SystemMode {
    SM_INITIALIZE,
    SM_SCANNING,
    SM_CONNECTING,
    SM_SERVICEDISCOVERY,
    SM_CONNECTED
} SystemMode_t;

static SystemMode_t systemMode = SM_INITIALIZE; // Starting mode of statemachine 
static int enabledCapsenseNotifications = 0;    // Have you enabled the notifiation for CapSense
static CYBLE_GAP_BD_ADDR_T remoteDevice;        // BD address of GATT Server
char buff[128];                                 // A scratch buffer

// UUID of CapsenseLED Service (from the GATT Server/Gap Peripheral
const uint8 CapLedService[] = { 0x03,0x03,0x9B,0x2C,
	                            0x11,0x07,0xF0,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00 };

/***************************************************************
 * Function to set the Capsense CCCD to get notifications
 **************************************************************/
void updateCapsenseNotification()
{
    CYBLE_GATTC_WRITE_REQ_T 	tempHandle;
    uint8 cccd=1;  
    enabledCapsenseNotifications = 1;
    
    tempHandle.attrHandle = cyBle_customCServ[CYBLE_CUSTOMC_LEDCAPSENSE_SERVICE_INDEX]
    .customServChar[CYBLE_CUSTOMC_LEDCAPSENSE_CAPSENSE_CHAR_INDEX]
    .customServCharDesc[CYBLE_CUSTOMC_LEDCAPSENSE_CAPSENSE_CAPSENSECCCD_DESC_INDEX].descHandle;
    
  	tempHandle.value.val = (uint8 *) &cccd;
    tempHandle.value.len = 1;
    
    CyBle_GattcWriteCharacteristicValue(cyBle_connHandle,&tempHandle);
}

CYBLE_GAPC_ADV_REPORT_T* scanReport;
CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *capsenseNTF;    

/* BLE App Callback Function */
void CyBle_AppCallback( uint32 eventCode, void *eventParam )
{
    
    switch( eventCode )
    {
        case CYBLE_EVT_STACK_ON:
            UART_UartPutString("CYBLE_EVT_STACK_ON\r\n");
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            systemMode = SM_SCANNING;
            enabledCapsenseNotifications = 0;
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST); // Start scanning for peripherals
            UART_UartPutString("Scanning...\r\n");
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:                     // Advertising packet
            scanReport = (CYBLE_GAPC_ADV_REPORT_T*)eventParam;
            if(scanReport->dataLen != 31) {                             // Number of bytes in ledcapsense advertising packet
                UART_UartPutString("x");
                break;
            }
            if(memcmp(&CapLedService,&scanReport->data[9],sizeof(CapLedService))) { // if service is in packet
                UART_UartPutString("m");
                return;
            }
                  
            // Setup for the connection
            remoteDevice.type = scanReport->peerAddrType;          // setup the BD addr
            memcpy(&remoteDevice.bdAddr,scanReport->peerBdAddr,6); // 6 bytes in BD addr
            systemMode = SM_CONNECTING;
            CyBle_GapcStopScan();                                  // stop scanning for peripherals
            UART_UartPutString(" Stop scan\r\n");
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP: // If you stopped scanning to make a connection.. then launch connection
            if(systemMode == SM_CONNECTING ) 
                CyBle_GapcConnectDevice(&remoteDevice);
                
            UART_UartPutString("CYBLE_EVT_GAPC_SCAN_START_STOP\r\n");
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:              // Connection request is complete
            CyBle_GattcStartDiscovery(cyBle_connHandle);  // Discover the services on the GATT Server
            systemMode = SM_SERVICEDISCOVERY;
            UART_UartPutString("CYBLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            break;
            
        case CYBLE_EVT_GATT_CONNECT_IND: // nothing to do
            UART_UartPutString("CYBLE_EVT_GATT_CONNECT_IND (do nothing)\r\n");
            break;

        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:  // Once you have a conenction set the CCCD and turn on the PWM
            systemMode = SM_CONNECTED;
            updateCapsenseNotification();
            UART_UartPutString("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE\r\n");
            break;
          
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:                                 // Capsense Notification Recevied
            capsenseNTF = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *)eventParam;
            if(capsenseNTF->handleValPair.value.val[0] == 0xFF) {               // Turn off the LED in no touch
                sprintf(buff,"CYBLE_EVT_GATTC_HANDLE_VALUE_NTF, val=0xff(0)\r\n");
            }else{
                sprintf(buff,"CYBLE_EVT_GATTC_HANDLE_VALUE_NTF, val=0x%x\r\n",capsenseNTF->handleValPair.value.val[0]);
            }
            UART_UartPutString(buff);
            break;
            
        case CYBLE_EVT_GATTC_WRITE_RSP: // Sucesfull write - nothing to do
            UART_UartPutString("CYBLE_EVT_GATTC_WRITE_RSP (do nothing)\r\n");
            break;

        default:
            sprintf(buff,"BLE: Unhandled event(0x%lx)\r\n",eventCode);
            UART_UartPutString(buff);
            break;
    }
}


int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_Start();
    CyBle_Start( CyBle_AppCallback );
    
    for(;;)
    {          
        CyBle_ProcessEvents();
        CyBle_EnterLPM(CYBLE_BLESS_DEEPSLEEP);    
    }
}
