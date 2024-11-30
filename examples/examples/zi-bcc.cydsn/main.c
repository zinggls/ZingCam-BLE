
#include "project.h"
#include <stdio.h>
#include <ZFrame.h>
#include <gitcommit.h>
#include <UartBuf.h>
#include <ZingUart.h>
#include <i2cs.h>
#include "imu.h"
#include "NoLog.h"
#include "bcc.h"

static uint16 writeCharVal = 0;

// Function to process data when a complete message is available
static void ZingCB(const char *buf)
{
    // Parsing the values into the structure
    if (!parse(ZCD,getZcdFrame(),buf)) {
#ifdef ZXX_DEBUG
        L("Parsing Error\r\n");
        L("Received: ");
        L(zing_status);
        L("\r\n");
#endif
    }
}

void SendCommandToPeripheral(uint8_t command) {
    // Use the characteristic index to obtain the attribute handle    
    CYBLE_GATT_DB_ATTR_HANDLE_T attrHandle = cyBle_customCServ[CYBLE_CUSTOMC_CUSTOM_SERVICE_SERVICE_INDEX]
                                                .customServChar[CYBLE_CUSTOMC_CUSTOM_SERVICE_CUSTOM_CHARACTERISTIC_CHAR_INDEX]
                                                .customServCharHandle;

    CYBLE_GATTC_WRITE_REQ_T writeReq;
    writeReq.attrHandle = attrHandle;
    writeReq.value.val = (uint8_t*)getZedFrame();
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
        i2cs_process(getZcdFrame());
        SendCommandToPeripheral(123);
        imu_process_uart_data(onImuFrame);
        
        CyDelay(10);
    }
}
