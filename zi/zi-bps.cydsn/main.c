#include <project.h>
#include <stdio.h>
#include <Log.h>
#include <UartBuf.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <Zing.h>
#include "Peripheral.h"
#include "imu.h"
#include "bps.h"
#include "led.h"
#include "icd.h"
#include "NoLog.h"
#include "i2cs.h"

static uint16 notifyCustom = 0;
static uint32 ZingCbCount = 0;

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    if(zxxKind==Unknown) zxxKind = detectZxx(buf);
    if(zxxKind!=ZED && zxxKind!=ZCH) return;

    // Parsing the values into the structure
    void *frame = &peripheral.zxxFrame;
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
    const char *name = (peripheral.zxxFrame.kind==ZCH)?"ZCH":"ZED";
    L("[ps %s] st:%d O>NC:%u(%04X) I>WRC=%u, %s USB:%d CNT:%d IvfCom{%d %d %d %d %d %d %d %d %d %d %d}\r\n", 
        GIT_INFO,cyBle_state,notifyCustom,zxxFrame.pos,getWritereqCustom(),name,zxxFrame.usb,zxxFrame.cnt,
        ivfCom.scopeCamera,
        ivfCom.scopeOutput,
        ivfCom.wirelessVideoChannelMode,
        ivfCom.wirelessVideoChannelInformation,
        ivfCom.scopeOperationMode,
        ivfCom.wirelessVideoTransmitterOperationModeStatus,
        ivfCom.wirelessVideoReceiverOperationModeStatus,
        ivfCom.wirelessVideoTransmitterImuOutputType,
        ivfCom.wirelessVideoTransmitterImuCalibrate,
        ivfCom.wirelssVideoReceiverImuOutputType,
        ivfCom.wirelessVideoReceiverImuCalibrate);
}

static uint8 sof = 1;                       //Euler angles as default
static uint8_t currentImuOutputType = 0x0;  //default
static uint8_t currentImuCalibrate = 0x0;   //default

static void processImuOutputType(uint8_t imuOutputType)
{
    if(imuOutputType != currentImuOutputType) {
        /*
        SET OUTPUT FORMAT, 1:Euler Angles, 2:Quaternion
        */
        sof = imuOutputType+1;  //ICD, 0x00 : Euler Angle (Default) 0x01 : Quaternion
        
        ImuFrame_setSof(sof);
        UART_IMU_InitializeOutputFormat(sof);
        currentImuOutputType = imuOutputType;
        
        CyDelay(1000);
    }
}

static void processImuCalibrate(uint8_t imuCalibrate)
{
    if(imuCalibrate != currentImuCalibrate) {
        /*
        0x00 : (default)
        0x01 : 자이로 보정
        0x02 : 가속도 보정
        0x03 : 지자계 보정
        0x04 : 지자계 보정 종료
        0x05 : 지자기 Off
        0x06 : 지자기 On
        0x07 : 지자기 능동형 On
        */
        
        bool bProcessed = false;
        switch(imuCalibrate) {
        case 0x0:
            break;
        case 0x1:   //자이로 보정
            UART_IMU_UartPutString("<cg>");
            bProcessed = true;
            break;
        case 0x2:   //가속도 보정
            UART_IMU_UartPutString("<cas>");
            bProcessed = true;
            break;
        case 0x3:   //지자계 보정
            UART_IMU_UartPutString("<cmf>");
            bProcessed = true;
            break;
        case 0x4:   //지자계 보정 종료
            UART_IMU_UartPutString(">");
            bProcessed = true;
            break;
        case 0x5:   //지자기 Off
            UART_IMU_UartPutString("<sem0>");
            bProcessed = true;
            break;
        case 0x6:   //지자기 On
            UART_IMU_UartPutString("<sem1>");
            bProcessed = true;
            break;
        case 0x7:   //지자기 능동형 On
            UART_IMU_UartPutString("<sem2>");
            bProcessed = true;
            break;
        default:
            break;
        }
        currentImuCalibrate = imuCalibrate;
        if(bProcessed) CyDelay(1000);
    }
}

static void processImuCommand(uint8_t imuOutputType,uint8_t imuCalibrate)
{
    processImuOutputType(imuOutputType);
    processImuCalibrate(imuCalibrate);
}

static short toShort(const char *data)
{
    return (short)((data[1] << 8) | data[0]);
}

static void onImuFrame(const ImuFrame *imu)
{
    IMU *i = &peripheral.imu;
    
    if(sof==1) {    //Euler
        i->imu1 = 0x0000;
        i->imu2 = toShort(imu->data+2);
        i->imu3 = toShort(imu->data+4);
        i->imu4 = toShort(imu->data+6);
        i->imu5 = toShort(imu->data+8);
        i->imuChecksum = toShort(imu->data+10);
    }
    
    if(sof==2) {    //Quaternion
        i->imu1 = toShort(imu->data+2);
        i->imu2 = toShort(imu->data+4);
        i->imu3 = toShort(imu->data+6);
        i->imu4 = toShort(imu->data+8);
        i->imu5 = toShort(imu->data+10);
        i->imuChecksum = toShort(imu->data+12);
    }
}

static void clearZxxZingInfo(ZXX_FRAME *z)
{
    z->kind = 0;
    z->usb = 0;
    z->bnd = ' ';    
    z->ppid = 0;
    z->devid = 0;
    z->fmt = 0;
    z->idx = 0;
    z->trt = ' ';
    z->ack = ' ';
    z->ppc = ' ';
    z->run = ' ';
    z->txid = 0;
    z->rxid = 0;
    z->cnt = 0;
    z->pos = 0;
}

static void updateStateInfo()
{
    peripheral.txState.txStateBattery = 78;   //TO DO 배터리 용량 파악하는 기능 구현전까지 하드코딩 상태로 둔다 (78%로 표시됨)
    
    uint8 zingState = getZingState();
    setZingState(zingState,0xE3,(uint8*)&peripheral.txState.txStateModem);     //수신기 모뎀 상태 0x00 : 정상, 0xE3 : 무선영상 송신기 모뎀 이상
    if(zingState==1) clearZxxZingInfo(&peripheral.zxxFrame);
    
    setImuState(getImuState(),0xE5,(uint8*)&peripheral.txState.txStateIMU);    //송신기 IMU 상태 0x00: 정상, 0xE5: 무선영상 수신기 IMU 이상
}

CY_ISR(Timer_ISR)
{
    // Clear the interrupt
    Timer_ClearInterrupt(Timer_INTR_MASK_TC);

    // Add your custom code here
    updateStateInfo();
    
    LED_RED_Write(!LED_RED_Read());
}

/***************************************************************
 * Main
 **************************************************************/
int main()
{
    CyGlobalIntEnable; 
    
    setGreen(LED_OFF);

    Zing_Init(ZingCB);
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    capsense_Start();
    capsense_InitializeEnabledBaselines();
    
    i2cs_start();
    /* Start BLE stack and register the callback function */
    CyBle_Start(BleCallBack);
    
    UART_IMU_StartAndInitialize();
    
    Timer_Start();
    Timer_ISR_StartEx(Timer_ISR);

    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE;
    
    uint32 count = 0;
    for(;;)
    {        
        /* if Capsense scan is done, read the value and start another scan */
        if(!capsense_IsBusy())
        {
            peripheral.zxxFrame.pos=capsense_GetCentroidPos(capsense_LINEARSLIDER0__LS);
            capsense_UpdateEnabledBaselines();
            capsense_ScanEnabledWidgets();
        }
        
        myDataHandle.value.val = (uint8_t *)&peripheral;
        myDataHandle.value.len = sizeof(PERIPHERAL);
        CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
        if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
        
        zxxLog();
   
        CyBle_ProcessEvents();
        zing_process_uart_data();
        i2cs_process();
        imu_process_uart_data(onImuFrame);
        processImuCommand(ivfCom.wirelessVideoTransmitterImuOutputType,ivfCom.wirelessVideoTransmitterImuCalibrate);
        
        count++;
    }
}
