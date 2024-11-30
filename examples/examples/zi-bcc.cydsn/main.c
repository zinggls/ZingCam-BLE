
#include "project.h"
#include <stdio.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <UartBuf.h>
#include <ZingUart.h>
#include <i2cs.h>
#include "imu.h"
#include "NoLog.h"

// Modes for a statemachine
typedef enum SystemMode {
    SM_INITIALIZE,
    SM_SCANNING,
    SM_CONNECTING,
    SM_SERVICEDISCOVERY,
    SM_CONNECTED
} SystemMode_t;

static SystemMode_t systemMode = SM_INITIALIZE; // Starting mode of statemachine 
static CYBLE_GAP_BD_ADDR_T remoteDevice;        // BD address of GATT Server
static uint16 writeCharVal = 0;
static uint16 notifiedCustom = 0;
static uint16 writeRsp = 0;
static CYBLE_GAPC_ADV_REPORT_T* scanReport;
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *notificationParam;
static ZED_FRAME zedFrame;
static ZCH_FRAME zchFrame;
static ZCD_FRAME zcdFrame;

// UUID of CapsenseLED Service (from the GATT Server/Gap Peripheral
const static uint8 CapLedService[] = { 0x03,0x03,0x9B,0x2C,
	                            0x11,0x07,0xF0,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00 };

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    // Parsing the values into the structure
    if (!parse(ZCD,&zcdFrame,buf)) {
#ifdef ZXX_DEBUG
        L("Parsing Error\r\n");
        L("Received: ");
        L(zing_status);
        L("\r\n");
#endif
    }
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
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST); // Start scanning for peripherals
            L("Scanning...\r\n");
            break;

        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:                     // Advertising packet
            scanReport = (CYBLE_GAPC_ADV_REPORT_T*)eventParam;
            if(scanReport->dataLen != 31) {                             // Number of bytes in ledcapsense advertising packet
                L("scanReport->dataLen(%d)!=31 not match\r\n",scanReport->dataLen);
                break;
            }
            if(memcmp(&CapLedService,&scanReport->data[9],sizeof(CapLedService))) { // if service is in packet
                L("CapLedService not match\r\n");
                return;
            }
                  
            // Setup for the connection
            remoteDevice.type = scanReport->peerAddrType;          // setup the BD addr
            memcpy(&remoteDevice.bdAddr,scanReport->peerBdAddr,6); // 6 bytes in BD addr
            systemMode = SM_CONNECTING;
            CyBle_GapcStopScan();                                  // stop scanning for peripherals
            L(" Stop scan\r\n");
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP: // If you stopped scanning to make a connection.. then launch connection
            if(systemMode == SM_CONNECTING ) 
                CyBle_GapcConnectDevice(&remoteDevice);
                
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
            {
                systemMode = SM_CONNECTED;
                CYBLE_API_RESULT_T apiRes = CyBle_GattcExchangeMtuReq(cyBle_connHandle, CYBLE_GATT_MTU);            
                L("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE, CyBle_GattcExchangeMtuReq=0x%x\r\n",apiRes);
            }
            break;
          
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:                                 // Capsense Notification Recevied
            notificationParam = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T*)eventParam;
            
            if(*getZxxKind()==Unknown) *getZxxKind() = inspect((char*)notificationParam->handleValPair.value.val);
            
            if(notificationParam->handleValPair.value.len == getFrameSize()) {
                notifiedCustom++;
                
                // Process the received data                                
                if(*getZxxKind()==ZED) memcpy(&zedFrame,notificationParam->handleValPair.value.val,notificationParam->handleValPair.value.len);
                if(*getZxxKind()==ZCH) memcpy(&zchFrame,notificationParam->handleValPair.value.val,notificationParam->handleValPair.value.len);
            }
            break;
            
        case CYBLE_EVT_GATTC_WRITE_RSP: // Sucesfull write - nothing to do
            writeRsp++;
            break;

        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            // MTU exchange response received
            {
                CYBLE_GATT_XCHG_MTU_PARAM_T* mtuRsp = (CYBLE_GATT_XCHG_MTU_PARAM_T*)eventParam;
                L("Negotiated MTU size: %d\n", mtuRsp->mtu);
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
    writeReq.attrHandle = attrHandle;
    writeReq.value.val = (uint8_t*)&zedFrame;
    writeReq.value.len = sizeof(ZED_FRAME);

    if(CyBle_GattcWriteCharacteristicValue(cyBle_connHandle, &writeReq)==CYBLE_ERROR_OK) writeCharVal++;
}

static void onImuFrame(const ImuFrame *imu)
{
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */
    
    ZingUart_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    i2cs_start();
    CyBle_Start( CyBle_AppCallback );
    UART_IMU_StartAndInitialize();
    
    for(;;)
    {          
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process(&zcdFrame);
        SendCommandToPeripheral(123);
        imu_process_uart_data(onImuFrame);
        
        CyDelay(10);
    }
}
