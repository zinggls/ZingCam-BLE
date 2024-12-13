#include "bcc.h"
#include "NoLog.h"
#include "i2cs.h"
#include "icd.h"
#include "led.h"

static SystemMode_t systemMode = SM_INITIALIZE; // Starting mode of statemachine 
static CYBLE_GAP_BD_ADDR_T remoteDevice;        // BD address of GATT Server
static uint16 writeCharVal = 0;
static uint16 notifiedCustom = 0;
static uint16 writeRsp = 0;
static CYBLE_GAPC_ADV_REPORT_T* scanReport;
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *notificationParam;
static ZXX_FRAME zxxFrame;
static ZCD_FRAME zcdFrame;

// UUID of CapsenseLED Service (from the GATT Server/Gap Peripheral
const static uint8 CapLedService[] = { 0x03,0x03,0x9B,0x2C,
	                            0x11,0x07,0xF0,0x34,0x9B,0x5F,0x80,0x00,0x00,0x80,0x00,0x10,0x00,0x00,0x00,0x00,0x00,0x00 };

ZCD_FRAME* getZcdFrame() { return &zcdFrame; }

SystemMode_t getSystemMode() { return systemMode; }

void setPairingState(SystemMode_t m, uint8 *buf)
{
    /*
    BLE 상태 (1 Byte)
    0x00 : 대기
    0x01 : 패어링 중
    0x02 : 패어링 완료
    */
    
    uint8 value = 0;    //SM_INITIALIZE
    if(m==SM_SCANNING || m==SM_CONNECTING || m==SM_SERVICEDISCOVERY) value = 1;
    if(m==SM_CONNECTED) value = 2;
    
    *buf = value;
}    

static void processingZxx()
{
    if(zxxKind==Unknown) zxxKind = inspect((char*)notificationParam->handleValPair.value.val);
    
    if(notificationParam->handleValPair.value.len == sizeof(ZXX_FRAME)) {
        notifiedCustom++;
        
        // Process the received data                                
        memcpy(&zxxFrame,notificationParam->handleValPair.value.val,notificationParam->handleValPair.value.len);
        setZxxBuffer(getI2CReadBuffer()+ZING_ZXX_OFFSET,&zxxFrame);
        mapZxxToICD(getI2CReadBuffer(),&zxxFrame);
        setImuBuffer(getI2CReadBuffer()+IMU_TX_OFFSET,&zxxFrame);   //ICD 무선영상 송신기 IMU
        getZcdFrame()->pos = zxxFrame.pos;
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
            memset(getI2CReadBuffer()+IMU_TX_OFFSET,0,IMU_TX_SIZE);
            memset(getI2CReadBuffer()+ZING_ZXX_OFFSET,0,ZING_ZXX_SIZE);
            L("Scanning...\r\n");
            setRGB(LED_OFF,LED_OFF,LED_OFF);
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
            setRGB(LED_OFF,LED_ON,LED_ON);
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
            setRGB(LED_OFF,LED_OFF,LED_ON);
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
            setRGB(LED_OFF,LED_ON,LED_OFF);
            break;
          
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:                                 // Capsense Notification Recevied
            notificationParam = (CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T*)eventParam;
            if(notificationParam->handleValPair.attrHandle == cyBle_customCServ[CYBLE_CUSTOMC_CUSTOM_SERVICE_SERVICE_INDEX]
                                                                .customServChar[CYBLE_CUSTOMC_CUSTOM_SERVICE_ZXX_CHAR_INDEX]
                                                                .customServCharHandle)
            {
                processingZxx();
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
            setRGB(LED_ON,LED_OFF,LED_OFF);
            break;
    }
}