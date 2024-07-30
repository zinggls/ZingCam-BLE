// iron

#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"
#include "ble.h"
#include "main.h"
#include "ai2c.h"
#include "aadc.h"
#include "icd.h"

#define BLE_TDD_SLOT 10

static uint32_t ZCBLE_systick = 0;
static uint32_t sw_ch_systick = 0;
static uint32_t itf_systick = 0;
static uint32_t auto_ch_systick = 0;
static uint32_t led_systick = 0;

static void ZCBLE_systick_isr(void);

#if HBLE
CY_ISR(SW_PW_EN_IRQ_Handler)
{
    PW_EN_Write(!(PW_EN_Read()));
    
    SW_PW_EN_ClearInterrupt();
}

CY_ISR(OTG_Detect_IRQ_Handler)
{
    PW_EN_Write(0);
    CyDelay(100);
    PW_EN_Write(1);
    
    OTG_Detect_ClearInterrupt();
}
#endif

int main(void)
{
    CyGlobalIntEnable;
    
    uint8_t write;
    uint16_t imu_values[NUM_TOTAL_IMU_QUATERNION_VALUES];
    uint8_t num_total_imu_values;
    //uint8_t i2c_data[AIAS_ICD_DATA_STRUCT_LENGTH];
#if HBLE
    uint8_t** zing_host_status_values;
#endif
#if DBLE
    uint8_t** zing_device_status_values;
    uint8_t imu_type;
#endif
    ZCBLE_frame zcble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;

    uint8_t type = 0;
    char run_status = 0;
	uint32_t tdd_tick = 0;
    uint32_t led_tick = 0;
        
    ZCBLE_init();
    IMU_init();
    AI2C_init();
    
#if HBLE
    AADC_init();
    zing_host_status_values = ZING_host_init();
    
    SW_PW_EN_IRQ_StartEx(SW_PW_EN_IRQ_Handler);
    OTG_Detect_IRQ_StartEx(OTG_Detect_IRQ_Handler);
#endif
#if DBLE    
    zing_device_status_values = ZING_device_init();
#endif

/*
    BIB_RST_N_Write(0);

    UART_IMU_UartPutString("<sor1>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sof1>");
    CyDelay(1000);
    UART_IMU_UartPutString("<soc2>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sots1>");
    CyDelay(1000);
    
    BIB_RST_N_Write(1);
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
        write = AIAS_ICD_write();
        
        CyBle_ProcessEvents();
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
        
        if (SW_LED_Read() == 1)
        {
            if (ZING_get_info() == ZING_INFO_CH1)
            {
                LED_USER0_Write(1);
            }
            else
            {
                LED_USER0_Write(0);
            }
            
            LED_USER1_Write(PW_EN_Read());
            
            if (ZING_get_ZED() == 1)
            {
                ZING_get_status(zing_host_status_values, 0, ZED_STATUS_RUN, (uint8_t*)&run_status);
            }
            else
            {
                run_status = ZING_get_host_status_run(zing_host_status_values);
            }
            
            if (run_status == 'Y')
            {                
                if (cyBle_state == CYBLE_STATE_CONNECTED)
                {
                    led_tick = 100;
                }
                else
                {
                    led_tick = 1000;
                }
                
                if (ZCBLE_systick - led_systick > led_tick)
                {
                    led_systick = ZCBLE_systick;
                    LED_USER2_Write(!(LED_USER2_Read()));
                }
            }
            else
            {
                if (cyBle_state == CYBLE_STATE_CONNECTED)
                {
                    LED_USER2_Write(1);
                }
            }
        }
        else
        {
            LED_USER0_Write(0);
            LED_USER1_Write(0);
            LED_USER2_Write(0);
        }  
#endif
#if DBLE
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
            imu_type = AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE);
            AIAS_ICD_set_receiver_imu_data(imu_type, imu_values);
        }
        
        AIAS_ICD_update_device_status(zcble_frame.status, zing_device_status_values);
#endif
        if (cyBle_state == CYBLE_STATE_CONNECTED)
        {
            if (CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
            {
                if (tdd_tick == 0)
                {
#if HBLE
                    tdd_tick = ZCBLE_systick;
#endif
#if DBLE
                    tdd_tick = ZCBLE_systick + (BLE_TDD_SLOT / 2);
#endif
                }
            
                if (write == 1)
                {
                    type = 1;
                }
                else
                {
                    zcble_frame.type = ZCBLE_READ;
                }
#if HBLE
                if (SW_CH_Read() == 0)
                {
                    if (ZCBLE_systick - sw_ch_systick > 1000)
                    {
                        if (ZING_get_info() == ZING_INFO_CH1)
                        {
                            ZING_set_channel_high();
                        }
                        else
                        {
                            ZING_set_channel_low();
                        }
                 
                        type = 1;
                        sw_ch_systick = ZCBLE_systick;
                    }
                }
                
                if (ZING_get_mode() == ZING_MODE_AUTO)
                {
                    if (ZING_get_itf() == 'Y')
                    {
                        if (ZCBLE_systick - itf_systick > 1000)
                        {   
                            if (ZCBLE_systick - auto_ch_systick > 1000)
                            {
                                if (ZING_get_info() == ZING_INFO_CH1)
                                {
                                    ZING_set_channel_high();
                                }
                                else
                                {
                                    ZING_set_channel_low();
                                }
                                
                                type = 1;
                                auto_ch_systick = ZCBLE_systick;
                            }
                            itf_systick = ZCBLE_systick;
                        }  
                    }
                }
                
                zcble_frame.icd_params.battey.transmitter = AADC_get_battery_level(AADC_measure(0));
                
                num_total_imu_values = (IMU_get_type() == 0) ? NUM_TOTAL_IMU_EULER_VALUES : NUM_TOTAL_IMU_QUATERNION_VALUES;
                memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * num_total_imu_values);
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
                
                if (ZCBLE_systick - tdd_tick > BLE_TDD_SLOT)
                {
                    if (type == 1)
                    {
                        zcble_frame.type = ZCBLE_WRITE;
                        type = 0;
                    }
                    
                    CyBle_GattsNotification(cyBle_connHandle, &notification);
                    tdd_tick = ZCBLE_systick;
                }
#endif 
#if DBLE
                AIAS_ICD_set(AIAS_BLE_STATUS, 0x02);
                
                num_total_imu_values = (IMU_get_type() == 0) ? NUM_TOTAL_IMU_EULER_VALUES : NUM_TOTAL_IMU_QUATERNION_VALUES;
                memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * num_total_imu_values);
                AIAS_ICD_set_zcble_frame(&zcble_frame);
                
                notification.attrHandle = 0x0001;
                notification.value.val = (uint8_t*)&zcble_frame;
                notification.value.len = sizeof(ZCBLE_frame);
                
                if (ZCBLE_systick - tdd_tick > BLE_TDD_SLOT)
                {
                    if (type == 1)
                    {
                        zcble_frame.type = ZCBLE_WRITE;
                        type = 0;
                    }
                    CyBle_GattsNotification(cyBle_connHandle, &notification);
                    tdd_tick = ZCBLE_systick;
                }
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
