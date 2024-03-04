#include <stdlib.h>

#include "ble.h"
#include "main.h"
#include "icd.h"

#if HBLE
static const uint8_t peripheral_address[CYBLE_GAP_BD_ADDR_SIZE] = { 0x01, 0x00, 0x00, 0x50, 0xA0, 0x00 };
static CYBLE_GAPC_ADV_REPORT_T advertisement_report;
static CYBLE_GAP_BD_ADDR_T device_address;
#endif
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T notification;
ZCBLE_frame zcble_frame;
#if HBLE
static uint8_t** zing_device_status_values;
#endif
#if DBLE
static char message[128];
static uint8_t** zing_host_status_values;
#endif

static void ZCBLE_callback(uint32_t event, void* parameters);

#if HBLE
uint8_t** ZCBLE_get_zing_device_status_values(void)
{
    return zing_device_status_values;
}
#endif

#if DBLE
uint8_t** ZCBLE_get_zing_host_status_values(void)
{
    return zing_host_status_values;
}
#endif

void ZCBLE_init(void)
{
#if HBLE
    zing_device_status_values = (uint8_t**)calloc(NUM_DEVICE_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
    {
        zing_device_status_values[i] = (uint8_t*)calloc(MAX_DATA_LENGTH, sizeof(uint8_t));
    }
#endif
#if DBLE
    zing_host_status_values = (uint8_t**)calloc(NUM_HOST_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
    {
        zing_host_status_values[i] = (uint8_t*)calloc(MAX_DATA_LENGTH, sizeof(uint8_t));
    }
#endif
    
    CyBle_Start(ZCBLE_callback);
}

void ZCBLE_callback(uint32_t event, void* parameters)
{
    switch (event)
    {
#if HBLE
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
        break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
        break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            memcpy(&advertisement_report, parameters, sizeof(CYBLE_GAPC_ADV_REPORT_T));
            
            if (memcmp(peripheral_address, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE) == 0)
            {
                CyBle_GapcStopScan();
                device_address.type = advertisement_report.peerAddrType;
                memcpy(device_address.bdAddr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
            }
        break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if (cyBle_state == CYBLE_STATE_DISCONNECTED)
            {
                CyBle_GapcConnectDevice(&device_address);
            }
        break;
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
        break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            CyBle_GattsWriteRsp(cyBle_connHandle);
        break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
        break;
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            memcpy(&notification, parameters, sizeof(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T));
            memcpy(&zcble_frame, notification.handleValPair.value.val, notification.handleValPair.value.len);
            for (uint8_t cnt = 0; cnt < NUM_DEVICE_STATUS; cnt++)
            {
                for (uint8_t idx = 0; idx < MAX_DATA_LENGTH; idx++)
                {
                    zing_device_status_values[cnt][idx] = zcble_frame.status_values[cnt * MAX_DATA_LENGTH + idx];
                }
            }
            
            if (zcble_frame.zing_params.reset == 1)
            {
                UART_ZING_PutChar(0x4);
                UART_ZING_PutChar('r');
                UART_ZING_PutChar('s');
                UART_ZING_PutChar('t');
            }
            
            if (zcble_frame.zing_params.auto_channel == 1)
            {
                ZING_change_channel(NULL, 1);
            }
            else
            {
                switch (zcble_frame.zing_params.set_channel)
                {
                    case 0x01:
                        ZING_set_channel_high();
                        break;
                    case 0x02:
                        ZING_set_channel_low();
                        break;
                }
            }
            
            switch (zcble_frame.zing_params.calibrate_imu)
            {
                case 0x01:
                    IMU_calibration_gyro();
                    break;
                case 0x02:
                    IMU_calibration_accelero_free();
                    break;
                case 0x03:
                    IMU_calibration_magneto_free();
                    break;
            }
                        
        break;
#endif
#if DBLE
        case CYBLE_EVT_STACK_ON:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x00);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x00);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattcStartDiscovery(cyBle_connHandle);
        break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattcExchangeMtuReq(cyBle_connHandle, MAX_BLE_FRAME_SIZE);
        break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattsWriteRsp(cyBle_connHandle);
        break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
        break;
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            memcpy(&notification, parameters, sizeof(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T));
            memcpy(&zcble_frame, notification.handleValPair.value.val, notification.handleValPair.value.len);
            
            for (uint8_t cnt = 0; cnt < NUM_HOST_STATUS; cnt++)
            {
                for (uint8_t idx = 0; idx < MAX_DATA_LENGTH; idx++)
                {
                    zing_host_status_values[cnt][idx] = zcble_frame.status_values[cnt * MAX_DATA_LENGTH + idx];
                }
            }
            
            if (zcble_frame.zing_params.zing_error == 1)
            {
                AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_MODEM_STATUS, 0xE3);
            }
            
            if (zcble_frame.zing_params.imu_error == 1)
            {
                AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_STATUS, 0xE5);
            }
            
            if (zcble_frame.zing_params.imu_output == IMU_EULER)
            {
                AIAS_ICD_set_transmitter_imu_data(IMU_EULER, zcble_frame.imu_values);
            }
            else if (zcble_frame.zing_params.imu_output == IMU_QUATERNION)
            {
                AIAS_ICD_set_transmitter_imu_data(IMU_QUATERNION, zcble_frame.imu_values);
            }
            
            P2_6_Write(!(P2_6_Read()));
            AIAS_ICD_update_host_status(zing_host_status_values);
            
           
            if (zcble_frame.zing_params.reset == 1)
            {
                UART_ZING_PutChar(0x4);
                UART_ZING_PutChar('r');
                UART_ZING_PutChar('s');
                UART_ZING_PutChar('t');
            }
            
            if (zcble_frame.zing_params.set_channel == 1)
            {
//              UART_ZING_PutChar(0x4);
//              UART_ZING_PutChar('r');
//              UART_ZING_PutChar('s');
//              UART_ZING_PutChar('t');
            }
            
            /*
            for (uint8_t i = 0; i < NUM_TOTAL_IMU_VALUES; i++)
            {
                sprintf(message, "%X, ", zcble_frame.imu_values[i]);
                UART_DBG_UartPutString(message);
            }
            UART_DBG_UartPutString("\r\n");
            
            for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
            {
                sprintf(message, "%s, ", zing_host_status_values[i]);
                UART_DBG_UartPutString(message);
            }
            UART_DBG_UartPutString("\r\n");
            */
        break;
#endif
    }
}