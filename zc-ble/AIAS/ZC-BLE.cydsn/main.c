#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"
#include "ble.h"
#include "main.h"
#include "ai2c.h"
#include "aadc.h"
#include "icd.h"

extern ZCBLE_frame zcble_frame;
static uint32_t ZCBLE_systick = 0;

static void ZCBLE_systick_isr(void);

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    //uint8_t i2c_data[AIAS_ICD_DATA_STRUCT_LENGTH];
#if HBLE
    uint8_t** zing_host_status_values;
    uint16_t sw_ch_timestamp_ms;
#endif
#if DBLE
    uint8_t** zing_device_status_values;
#endif
    ZCBLE_frame zcble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
        
    ZCBLE_init();
    IMU_init();
    AI2C_init();
    
#if HBLE
    AADC_init();
    zing_host_status_values = ZING_host_init();
    sw_ch_timestamp_ms = 0;
#endif
#if DBLE    
    zing_device_status_values = ZING_device_init();
#endif

    /*
    UART_IMU_UartPutString("<sor0>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sof1>");
    CyDelay(1000);
    UART_IMU_UartPutString("<soc2>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sots1>");
    CyDelay(1000);
    */

    CySysTickStart();
    
    for (uint8_t i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            CySysTickSetCallback(i, ZCBLE_systick_isr);
            break;
        }
    }

#if DBLE
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE, IMU_EULER);
#endif

    while (1)
    {
        AIAS_ICD_read();
        AIAS_ICD_write();
        
        CyBle_ProcessEvents();
                
        if (cyBle_state == CYBLE_STATE_CONNECTED)
        {
            if (CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
            {
                memset(&zcble_frame, 0, sizeof(ZCBLE_frame));
#if HBLE
                if (ZING_parse_host_status(zing_host_status_values) != 1)
                {
                    zcble_frame.icd_params.modem.transmitter = 0x03;
                }
                else
                {
                    zcble_frame.icd_params.modem.transmitter = 0x00;
                }
                
                if (IMU_get(imu_values) != 1)
                {
                    zcble_frame.icd_params.tx_imu.status = 0x05;
                }
                else
                {
                    zcble_frame.icd_params.tx_imu.status = 0x00;
                }
                
                zcble_frame.icd_params.battey.transmitter = AADC_get_battery_level(AADC_measure(0));
                
                memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
                AIAS_ICD_set_zcble_frame(&zcble_frame);
                
                if (ZING_get_ZED() == 1)
                {
                    zcble_frame.status = ZING_zed_set_status(zing_host_status_values);
                }
                else
                {
                    zcble_frame.status = ZING_zch_set_status(zing_host_status_values);
                    zcble_frame.icd_params.scope.camera = 0x04;
                }
               
                notification.attrHandle = 0x0001;
                notification.value.val = (uint8_t*)&zcble_frame;
                notification.value.len = sizeof(ZCBLE_frame);
                CyBle_GattsNotification(cyBle_connHandle, &notification);
                
#endif
#if DBLE
                AIAS_ICD_set(AIAS_BLE_STATUS, 0x02);
                AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_OPERATION_MODE_STATUS, 1);
                
                if (ZING_parse_device_status(zing_device_status_values) != 1)
                {
                    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_MODEM_STATUS, 0xE4);
                }
                else
                {
                    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_MODEM_STATUS, 0x00);   
                }
                
                zcble_frame.status = ZING_zcd_set_status(zing_device_status_values);
                
                if (IMU_get(imu_values) != 1)
                {
                    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_STATUS, 0xE6);
                }
                else
                {
                    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_STATUS, 0x00);
                }
                
                AIAS_ICD_update_device_status(zcble_frame.status, zing_device_status_values);
                
                memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
                AIAS_ICD_set_zcble_frame(&zcble_frame);
                
                notification.attrHandle = 0x0001;
                notification.value.val = (uint8_t*)&zcble_frame;
                notification.value.len = sizeof(ZCBLE_frame);
                CyBle_GattsNotification(cyBle_connHandle, &notification);

                AIAS_ICD_set_receiver_imu_data(IMU_EULER, imu_values);
#endif
            }
        }
    }
}

void ZCBLE_systick_isr(void)
{
    ZCBLE_systick = ZCBLE_systick + 1;
}

uint32_t ZCBLE_get_systick(void)
{
    return ZCBLE_systick;
}
