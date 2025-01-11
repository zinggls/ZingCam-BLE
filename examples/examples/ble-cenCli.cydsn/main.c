
#include "project.h"
#include <stdio.h>
#include <stdarg.h>
#include <stdbool.h>
#include <FlashRow.h>

#define LOG_BUFFER_SIZE 1024

static char buffer[LOG_BUFFER_SIZE];

void L(const char *format, ...)
{
    va_list args;

    // Initialize the variable argument list
    va_start(args, format);

    // Format the string
    vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);

    // End using variable argument list
    va_end(args);

    // Send the formatted string over UART
    UART_DBG_UartPutString(buffer);
}

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

// UUID of CapsenseLED Service (from the GATT Server/Gap Peripheral
const uint8 CapLedService[] = { 0x03,0x03,0x9B,0x2C,
	                            0x11,0x07,0xF0,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00 };

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

ulong writeCharVal = 0;
ulong notifiedCustom = 0;
ulong writeRsp = 0;

uint8_t capsensePos = 0xFF;

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
CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *notificationParam;
bool isAddressStored;

void printAddress(CYBLE_GAP_BD_ADDR_T *addr)
{
    L("[%d] ",addr->type);
    for (unsigned int i = 0; i < CYBLE_GAP_BD_ADDR_SIZE; i++)
    {
        L("%02X", addr->bdAddr[i]);
        if (i < CYBLE_GAP_BD_ADDR_SIZE - 1) L(":");
    }
}

int cmpAddr(CYBLE_GAP_BD_ADDR_T *addr1,CYBLE_GAP_BD_ADDR_T *addr2)
{
    L("Comparing stored addr ");
    printAddress(addr1);
    L(" and ");
    L("scanned addr ");
    printAddress(addr2);
    L("\r\n");
    
    if(addr1->type != addr2->type) return -1;    
    return memcmp(addr1->bdAddr, addr2->bdAddr, CYBLE_GAP_BD_ADDR_SIZE);
}

void setupForConnection()
{
    L("setup for connection\r\n");
    remoteDevice.type = scanReport->peerAddrType;          // setup the BD addr
    memcpy(&remoteDevice.bdAddr,scanReport->peerBdAddr,6); // 6 bytes in BD addr
    systemMode = SM_CONNECTING;
    CyBle_GapcStopScan();                                  // stop scanning for peripherals
    L("Stop scan\r\n");
}

/* BLE App Callback Function */
void CyBle_AppCallback( uint32 eventCode, void *eventParam )
{
    
    switch( eventCode )
    {
        case CYBLE_EVT_STACK_ON:
            L("CYBLE_EVT_STACK_ON\r\n");
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            systemMode = SM_SCANNING;
            enabledCapsenseNotifications = 0;
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST); // Start scanning for peripherals
            L("Scanning...\r\n");
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:                     // Advertising packet
            scanReport = (CYBLE_GAPC_ADV_REPORT_T*)eventParam;
            if(scanReport->dataLen != 31) {                             // Number of bytes in ledcapsense advertising packet
                //L("x");
                break;
            }
            if(memcmp(&CapLedService,&scanReport->data[9],sizeof(CapLedService))) { // if service is in packet
                //L("m");
                return;
            }
            
            L("RSSI = %d, Address = ", scanReport->rssi);
            for (unsigned int i = 0; i < CYBLE_GAP_BD_ADDR_SIZE; i++)
            {
                L("%02X", scanReport->peerBdAddr[i]);
                if (i < CYBLE_GAP_BD_ADDR_SIZE - 1) L(":");
            }
            L("\n");
            
            CYBLE_GAP_BD_ADDR_T addr;
            memcpy(&addr.bdAddr,scanReport->peerBdAddr,CYBLE_GAP_BD_ADDR_SIZE);
            addr.type = scanReport->peerAddrType;
            if(isAddressStored) {
                if(cmpAddr(&flashData.bdAddr,&addr)==0) {
                    printAddress(&addr);
                    L(" Device found in whitelist\r\n");
                    setupForConnection();             
                }else{
                    printAddress(&addr);
                    L(" Device not found in Whitelist\r\n");
                }
            }else{
                L("Empty Whitelist\r\n");
                setupForConnection();
            }
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP: // If you stopped scanning to make a connection.. then launch connection
            if(systemMode == SM_CONNECTING ) {
                if(CyBle_GapcConnectDevice(&remoteDevice)==CYBLE_ERROR_OK) {
                    if (CyBle_GapAddDeviceToWhiteList(&remoteDevice) == CYBLE_ERROR_OK) {
                        if(!isAddressStored) {
                            printAddress(&remoteDevice);
                            cystatus status;
                            if(SavePeripheralAddress(&remoteDevice,&status)) {
                                L(" Address saved in flash\r\n");
                            }else{
                                L(" Address failed to save in flash(0x%02X)\r\n",status);
                            }
                        }
                    }
                }
            }
                
            L("CYBLE_EVT_GAPC_SCAN_START_STOP\r\n");
            break;

        case CYBLE_EVT_GAP_DEVICE_CONNECTED:              // Connection request is complete
            CyBle_GattcStartDiscovery(cyBle_connHandle);  // Discover the services on the GATT Server
            systemMode = SM_SERVICEDISCOVERY;
            L("CYBLE_EVT_GAP_DEVICE_CONNECTED\r\n");
            break;
            
        case CYBLE_EVT_GATT_CONNECT_IND: // nothing to do
            L("CYBLE_EVT_GATT_CONNECT_IND (do nothing)\r\n");
            break;

        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:  // Once you have a conenction set the CCCD and turn on the PWM
            systemMode = SM_CONNECTED;
            updateCapsenseNotification();
            L("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE\r\n");
            break;
          
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:                                 // Capsense Notification Recevied
            notificationParam = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T*)eventParam;
            
            if(notificationParam->handleValPair.value.len == sizeof(MyData)) {
                notifiedCustom++;
                
                MyData* receivedData = (MyData*)notificationParam->handleValPair.value.val;
                // Process the received data
                capsensePos = receivedData->val1;
#if _VERBOSE  
                uint8_t val2 = receivedData->val2;
                sprintf(buff,"val1=0x%x val2=0x%x \r\n",val1,val2);
                UART_UartPutString(buff);
#endif
            }
            break;
            
        case CYBLE_EVT_GATTC_WRITE_RSP: // Sucesfull write - nothing to do
#if _VERBOSE
            UART_UartPutString("CYBLE_EVT_GATTC_WRITE_RSP (do nothing)\r\n");
#endif
            writeRsp++;
            break;

        default:
            L("BLE: Unhandled event(0x%lx)\r\n",eventCode);
            break;
    }
}

void SendCommandToPeripheral(uint8_t command) {
    // Use the characteristic index to obtain the attribute handle    
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle = cyBle_customCServ[CYBLE_CUSTOMC_CUSTOM_SERVICE_SERVICE_INDEX]
                                                .customServChar[CYBLE_CUSTOMC_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_INDEX]
                                                .customServCharHandle;

    CYBLE_GATTC_WRITE_REQ_T writeReq;
    data.val1 = command;
    writeReq.attrHandle = attrHandle;
    writeReq.value.val = (uint8_t*)&data;
    writeReq.value.len = sizeof(MyData);

    CYBLE_API_RESULT_T res = CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeReq);
    if(res!=CYBLE_ERROR_OK){
#if _VERBOSE
        sprintf(buff,"CyBle_GattcWriteCharacteristicValue error=0x%x\r\n",res);
        UART_UartPutString(buff);
#endif
    }else{
        writeCharVal++;
#if _VERBOSE
        UART_UartPutString("SendCommandToPeripheral\r\n");
#endif
    }
}

CY_ISR( Pin_SW2_Handler )
{   
    Pin_Red_Write( ~Pin_Red_Read() );
    
    if(Pin_SW2_Read()==0) { // Button pressed
        //do nothing yet
    }else{ // Button released
        cystatus status;
        if(ClearPeripheralAddress(&status)) {
            L("Address in flash cleared\r\n");
            CyDelay(100);
            CySoftwareReset();
        }else{
             L("Clearing Address in flash failed(0x%02X)\r\n",status);
        }
    }
    
    Pin_SW2_ClearInterrupt();
}

int main(void)
{    
    CyGlobalIntEnable; /* Enable global interrupts. */
    UART_DBG_Start();
    CyBle_Start( CyBle_AppCallback );
    Pin_SW2_Int_StartEx( Pin_SW2_Handler );
    
    //Load stored address from flash
    if(LoadStoredPeripheralAddress(&flashData)) {
        isAddressStored = true;
        printAddress(&flashData.bdAddr);
        L(" loaded from flash\r\n");
    }else{
        isAddressStored = false;
        L("could not load from empty flash\r\n");
    }
    
    for(;;)
    {          
        CyBle_ProcessEvents();
        SendCommandToPeripheral(123);
        
#ifndef _VERBOSE
        L("[ble-cenCli] SM:%d cyBle_state:0x%x OUT:WriteCharVal=%lu    IN:Notified { Custom=%lu,WriteRsp=%lu,CapsensePos=%d }\r", systemMode,cyBle_state,writeCharVal ,notifiedCustom,writeRsp,capsensePos);
#endif
        CyDelay(10);
    }
}
