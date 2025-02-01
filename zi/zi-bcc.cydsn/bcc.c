#include "bcc.h"
#include "NoLog.h"
#include "i2cs.h"
#include "icd.h"
#include "led.h"
#include "Peripheral.h"
#include "FlashRow.h"

static SystemMode_t systemMode = SM_INITIALIZE; // Starting mode of statemachine 
static CYBLE_GAP_BD_ADDR_T remoteDevice;        // BD address of GATT Server
static uint16 writeCharVal = 0;
static uint16 notifiedCustom = 0;
static uint16 writeRsp = 0;
static CYBLE_GAPC_ADV_REPORT_T* scanReport;
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T *notificationParam;
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

static void mapToICD(uint8_t *buf,PERIPHERAL *p)
{
    *(buf+ICD_SCOPE_VIDEO_KIND_OFFSET) = p->scope.scopeStateKind;
    *(buf+ICD_SCOPE_OUTPUT_OFFSET) = p->scope.scopeStateOut;
    *(buf+ICD_SCOPE_BATTERY_OFFSET) = p->scope.scopeStateBattery;
    *(buf+ICD_SCOPE_IR_STATE_OFFSET) = p->scope.scopeStateIR;
    *(buf+ICD_SCOPE_EO_STATE_OFFSET) = p->scope.scopeStateEO;

    *(buf+ICD_TX_BATTERY_OFFSET) = p->txState.txStateBattery;
    *(buf+ICD_TX_MODEM_STATE_OFFSET) = p->txState.txStateModem;
    *(buf+ICD_TX_IMU_STATE_OFFSET) = p->txState.txStateIMU;
}

static uint8_t *setImuBuffer(uint8_t *buf,IMU *i)
{
    uint8_t *ptr = buf; // Pointer to traverse the buffer

    memcpy(ptr, &i->imu1, sizeof(i->imu1));                     ptr += sizeof(i->imu1);
    memcpy(ptr, &i->imu2, sizeof(i->imu2));                     ptr += sizeof(i->imu2);
    memcpy(ptr, &i->imu3, sizeof(i->imu3));                     ptr += sizeof(i->imu3);
    memcpy(ptr, &i->imu4, sizeof(i->imu4));                     ptr += sizeof(i->imu4);
    memcpy(ptr, &i->imu5, sizeof(i->imu5));                     ptr += sizeof(i->imu5);
    memcpy(ptr, &i->imuChecksum, sizeof(i->imuChecksum));       ptr += sizeof(i->imuChecksum);

    return ptr;
}

static void setScope(uint8_t *buf,SCOPE *s)
{
    /*
    "지능형 영상융합처리기(Master) - 무선영상 수신기" Read Buffer에 값 설정
    */
    buf[0] = s->scopeKindChangeNotify;  //[0] 화기조준경 영상 종류 변경 알림
    buf[1] = s->scopeOutChangeNotify;   //[1] 화기조준경 영상 출력종류 변경 알림
    buf[4] = s->scopeOperMode;          //[4] 화기조준경 운용모드 상태
    buf[11] = s->scopeStateKind;        //[11] 화기조준경 영상 종류
    buf[12] = s->scopeStateOut;         //[12] 화기조준경 영상 상태
    buf[13] = s->scopeStateBattery;     //[13] 화기조준경 배터리 잔량
    buf[15] = s->scopeStateIR;          //[15] 화기조준경 IR 모듈 상태
    buf[16] = s->scopeStateEO;          //[16] 화기조준경 EO 모듈 상태
}

static void setBpsVerBuffer(uint8_t *buf,Version *v)
{
    memcpy(buf,v->info,VERSION_SIZE);
}

static void setZxxVerBuffer(uint8_t *buf,Version *v)
{
    memcpy(buf,v->info,VERSION_SIZE);
}

static void setScopeVideoKind(uint8_t *buf,int kind)
{
    switch(kind){
    case 2: //ZCH
        *buf = 0x04;    //경II인 경우에만 0x04로 표시한다. ref.(AIAS)연동통제문서(ICD)_v0.3_241231.xls
        break;
    default:
        //경II가 아닌 경우이므로 이들은 mapToICD에서 복사된 값을 유지한다
        break;
    }
}

static void setScopeOutput(uint8_t *buf,char run)
{
    if(run=='Y') {
        *buf = 0x0;     //화기조준경 영상 상태 0x00 : 출력     ref.(AIAS)연동통제문서(ICD)_v0.3_241231.xls
    }else{
        *buf = 0x1;     //화기조준경 영상 상태 0x01 : 미출력    ref.(AIAS)연동통제문서(ICD)_v0.3_241231.xls
    }
}

static void processingZxx()
{
    if(zxxKind==Unknown) zxxKind = inspect((char*)notificationParam->handleValPair.value.val);
    
    if(notificationParam->handleValPair.value.len == sizeof(PERIPHERAL)) {
        notifiedCustom++;
        
        // Process the received data                                
        memcpy(&peripheral,notificationParam->handleValPair.value.val,notificationParam->handleValPair.value.len);
        setScope(getI2CReadBuffer(),&peripheral.scope);
        setZxxBuffer(getI2CReadBuffer()+ZING_ZXX_OFFSET,&peripheral.zxxFrame);
        mapToICD(getI2CReadBuffer(),&peripheral);
        setImuBuffer(getI2CReadBuffer()+IMU_TX_OFFSET,&peripheral.imu);   //ICD 무선영상 송신기 IMU
        getZcdFrame()->pos = peripheral.zxxFrame.pos;
        setBpsVerBuffer(getI2CReadBuffer()+I2C_IVF_READ_BUFFER_SIZE+2*VERSION_SIZE,&peripheral.bpsVer);
        setZxxVerBuffer(getI2CReadBuffer()+I2C_IVF_READ_BUFFER_SIZE+3*VERSION_SIZE,&peripheral.zxxVer);
        
        setScopeVideoKind(getI2CReadBuffer()+ICD_SCOPE_VIDEO_KIND_OFFSET,peripheral.zxxFrame.kind);
        setScopeOutput(getI2CReadBuffer()+ICD_SCOPE_OUTPUT_OFFSET,peripheral.zxxFrame.run);
    }
}

void printAddress(const CYBLE_GAP_BD_ADDR_T *addr)
{
    L("[%d] ",addr->type);
    for (unsigned int i = 0; i < CYBLE_GAP_BD_ADDR_SIZE; i++)
    {
        L("%02X", addr->bdAddr[i]);
        if (i < CYBLE_GAP_BD_ADDR_SIZE - 1) L(":");
    }
}

int cmpAddr(const CYBLE_GAP_BD_ADDR_T *addr1,const CYBLE_GAP_BD_ADDR_T *addr2)
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

void RetrieveStoredPeripheralAddress()
{
    const FlashData_t* fd = LoadStoredPeripheralAddress();
    if(fd) {
        printAddress(&fd->bdAddr);
        L(" loaded from flash\r\n");
    }else{
        L("could not load from empty flash\r\n");
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
                  
            CYBLE_GAP_BD_ADDR_T addr;
            memcpy(&addr.bdAddr,scanReport->peerBdAddr,CYBLE_GAP_BD_ADDR_SIZE);
            addr.type = scanReport->peerAddrType;
            if(IsAddressStored()) {
                if(cmpAddr(&GetFlashData()->bdAddr,&addr)==0) {
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
            setRGB(LED_OFF,LED_ON,LED_ON);
            break;

        case CYBLE_EVT_GAPC_SCAN_START_STOP: // If you stopped scanning to make a connection.. then launch connection
            if(systemMode == SM_CONNECTING ) {
                if(CyBle_GapcConnectDevice(&remoteDevice)==CYBLE_ERROR_OK) {
                    if (CyBle_GapAddDeviceToWhiteList(&remoteDevice) == CYBLE_ERROR_OK) {
                        if(!IsAddressStored()) {
                            printAddress(&remoteDevice);
                            cystatus status;
                            if(SavePeripheralAddress(&remoteDevice,&status)) {
                                for(int i=0;i<5;i++) {
                                    LED_GREEN_Write(!LED_GREEN_Read()); //loaded from flash
                                    CyDelay(100);
                                }
                                L(" Address saved in flash\r\n");
                                RetrieveStoredPeripheralAddress();
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