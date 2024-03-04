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

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    uint8_t** zing_host_status_values;
    uint8_t** zing_device_status_values;
    uint8_t i2c_data[AIAS_ICD_DATA_STRUCT_LENGTH];
#if HBLE
    char zing_host_status[MAX_BUFFER_LENGTH];
#endif
#if DBLE
    char message[128];
    char zing_device_status[MAX_BUFFER_LENGTH];
    uint8_t rst;
    uint8_t host_imu_output;
    uint8_t host_imu_calibrate;
    uint8_t auto_channel;
    uint8_t set_channel_num;
#endif
    ZCBLE_frame zcble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
        
    ZCBLE_init();
    IMU_init();
    AI2C_init();
    
#if HBLE
    AADC_init();
    
    zing_host_status_values = ZING_host_init();
    zing_device_status_values = ZCBLE_get_zing_device_status_values();
#endif
#if DBLE    
    zing_host_status_values = ZCBLE_get_zing_host_status_values();
    zing_device_status_values = ZING_device_init();
    rst = 0;
    auto_channel = 0;
    set_channel_num = 0;
    host_imu_output = 0;
    host_imu_calibrate = 0;
#endif

    P2_6_Write(1);
    RF_LNA_0_Write(1);
    RF_LNA_1_Write(1);

    IMU_set_output_gyro(0);
    CyDelay(1000);
    IMU_set_output_accelero(0);
    CyDelay(1000);
    IMU_set_output_mageneto(0);
    CyDelay(1000);
    IMU_set_output_timestamp(1);
    CyDelay(1000);
    
    while (1)
    {
#if DBLE
        if (AI2C_read(i2c_data, AIAS_ICD_DATA_STRUCT_LENGTH) == 0)
        {
            if (i2c_data[AIAS_ICD_RW] == AIAS_ICD_READ)
            {
                AI2C_write(AIAS_ICD_get_map() + i2c_data[AIAS_ICD_ADDR], i2c_data[AIAS_ICD_LENGTH]);
            }
            else if (i2c_data[AIAS_ICD_RW] == AIAS_ICD_WRITE)
            {                
                if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_CHANNEL_MODE) && (i2c_data[AIAS_ICD_DATA] == 0x01))
                {
                    auto_channel = 1;
                }
                else if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_CHANNEL_MODE) && (i2c_data[AIAS_ICD_DATA] == 0x02))
                {
                    auto_channel = 0;
                }
                
                if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_CHANNEL_INFORMATION) && (i2c_data[AIAS_ICD_DATA] == 0x01))
                {
                    set_channel_num = 0x01;
                }
                else if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_CHANNEL_INFORMATION) && (i2c_data[AIAS_ICD_DATA] == 0x02))
                {
                    set_channel_num = 0x02;
                }
                
                if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_TRANSMITTER_IMU_OUTPUT_TYPE) && (i2c_data[AIAS_ICD_DATA] == IMU_EULER))
                {
                    host_imu_output = 0x00;
                }
                else if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_TRANSMITTER_IMU_OUTPUT_TYPE) && (i2c_data[AIAS_ICD_DATA] == IMU_QUATERNION))
                {
                    host_imu_output = 0x01;
                }
                
                
                if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE) && (i2c_data[AIAS_ICD_DATA] == IMU_EULER))
                {
                    IMU_set_output_format(IMU_EULER);
                }
                else if ((i2c_data[AIAS_ICD_ADDR] == WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE) && (i2c_data[AIAS_ICD_DATA] == IMU_QUATERNION))
                {
                    IMU_set_output_format(IMU_QUATERNION);
                }
                
                if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_TRANSMITTER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x01))
                {
                    host_imu_calibrate = 0x01;
                }
                else if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_TRANSMITTER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x02))
                {
                    host_imu_calibrate = 0x02;
                }
                else if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_TRANSMITTER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x03))
                {
                    host_imu_calibrate = 0x03;
                }
                
                if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x01))
                {
                    IMU_calibration_gyro();
                }
                else if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x02))
                {
                    IMU_calibration_accelero_free();
                }
                else if ((i2c_data[AIAS_ICD_RW] == WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE) && (i2c_data[AIAS_ICD_DATA] == 0x03))
                {
                    IMU_calibration_magneto_free();
                }
                
                memcpy(AIAS_ICD_get_map() + i2c_data[AIAS_ICD_ADDR], &i2c_data[AIAS_ICD_DATA], i2c_data[AIAS_ICD_LENGTH]);
            }
        }
#endif
        
        CyBle_ProcessEvents();
        
        if (cyBle_state == CYBLE_STATE_CONNECTED)
        {
            if (CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
            {
                memset(&zcble_frame, 0, sizeof(ZCBLE_frame));
#if HBLE
                if (ZING_get_host_status(zing_host_status) == 1)
                {
                    if (ZING_parse_host_status(zing_host_status, zing_host_status_values) != 1)
                    {
                        zcble_frame.zing_params.zing_error = 1;
                    }
                    else
                    {
                        zcble_frame.zing_params.zing_error = 0;
                        for (uint8_t cnt = 0; cnt < (NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS); cnt++)
                        {
                            for (uint8_t idx = 0; idx < MAX_DATA_LENGTH; idx++)
                            {
                                zcble_frame.status_values[cnt * MAX_DATA_LENGTH + idx] = zing_host_status_values[cnt][idx];
                            }
                        }
                    }
                    
                    if (IMU_get(imu_values) != 1)
                    {
                        zcble_frame.zing_params.imu_error = 1;
                    }
                    else
                    {
                        zcble_frame.zing_params.imu_error = 0;
                        memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
                    }
                    
                    ZING_change_channel(zing_host_status_values, 0);
                    
                    notification.attrHandle = 0x0001;
                    notification.value.val = (uint8_t*)&zcble_frame;
                    notification.value.len = sizeof(ZCBLE_frame);
                    CyBle_GattsNotification(cyBle_connHandle, &notification);
                    
                    P2_6_Write(!(P2_6_Read()));                 
                }
#endif
#if DBLE
                AIAS_ICD_set(AIAS_BLE_STATUS, 0x02);
                AIAS_ICD_update_device_status(zing_device_status_values);
            
                if (ZING_get_device_status(zing_device_status) == 1)
                {
                    if (ZING_parse_device_status(zing_device_status, zing_device_status_values) != 1)
                    {
                        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_MODEM_STATUS, 0xE4);
                    }
                    else
                    {
                        for (uint8_t cnt = 0; cnt < (NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS); cnt++)
                        {
                            for (uint8_t idx = 0; idx < MAX_DATA_LENGTH; idx++)
                            {
                                zcble_frame.status_values[cnt * MAX_DATA_LENGTH + idx] = zing_device_status_values[cnt][idx];
                            }
                        }
                    }
                    
                    if (IMU_get(imu_values) != 1)
                    {
                        //UART_DBG_UartPutString("Get IMU datas failed\r\n");
                        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_STATUS, 0xE6);
                    }
                    else
                    {
                        memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
                        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_STATUS, 0x00);
                    }
                    
                    if (rst == 1)
                    {
                        rst = 0;
                        zcble_frame.zing_params.reset = 1;
                    }
                    else
                    {
                        zcble_frame.zing_params.reset = 0;
                    }
                    
                    if ((strcmp((char*)zing_device_status_values[DEVICE_STATUS_ITF], "Y" ) == 0) && (auto_channel == 1))
                    {
                        set_channel_num = 1;
                    }
                    
                    if (auto_channel == 1)
                    {
                        zcble_frame.zing_params.auto_channel = 1;
                    }
                    
                    if (set_channel_num == 0x01)
                    {
                        set_channel_num = 0;
                        zcble_frame.zing_params.set_channel = 0x01;
                    }
                    else if (set_channel_num == 0x02)
                    {
                        set_channel_num = 0;
                        zcble_frame.zing_params.set_channel = 0x02;
                    }
                    else
                    {
                        set_channel_num = 0;
                        zcble_frame.zing_params.set_channel = 0x00;
                    }
                    
                    if (host_imu_output == 0x00)
                    {
                        zcble_frame.zing_params.imu_output = 0;
                    }
                    else if (host_imu_output == 0x01)
                    {
                        zcble_frame.zing_params.imu_output = 1;
                    }
                    
                    if (host_imu_calibrate == 0x01)
                    {
                        host_imu_calibrate = 0;
                        zcble_frame.zing_params.calibrate_imu = 0x01;
                    }
                    else if (host_imu_calibrate == 0x02)
                    {
                        host_imu_calibrate = 0;
                        zcble_frame.zing_params.calibrate_imu = 0x02;
                    }
                    else if (host_imu_calibrate == 0x03)
                    {
                        host_imu_calibrate = 0;
                        zcble_frame.zing_params.calibrate_imu = 0x03;
                    }
                    
                    notification.attrHandle = 0x0001;
                    notification.value.val = (uint8_t*)&zcble_frame;
                    notification.value.len = sizeof(ZCBLE_frame);
                    CyBle_GattsNotification(cyBle_connHandle, &notification);

                    AIAS_ICD_set_receiver_imu_data(IMU_EULER, imu_values);
                    
                    /*
                    ch = UART_DBG_UartGetChar();
                    
                    if (ch == '&')
                    {
                        UART_DBG_UartPutString("D,");
                        for (uint8_t i = 0; i < NUM_TOTAL_IMU_VALUES; i++)
                        {
                            sprintf(message, "%d,", imu_values[i]);
                            UART_DBG_UartPutString(message);
                        }
                        for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
                        {
                            sprintf(message, "%s,", zing_device_status_values[i]);
                            UART_DBG_UartPutString(message);
                        }
                        UART_DBG_UartPutString("\r\n");
                    }
                    else if (ch == '*')
                    {
                        UART_DBG_UartPutString("H,");
                        for (uint8_t i = 0; i < NUM_TOTAL_IMU_VALUES; i++)
                        {
                            sprintf(message, "%d,", zcble_frame.imu_values[i]);
                            UART_DBG_UartPutString(message);
                        }
                        for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
                        {
                            sprintf(message, "%s,", zing_host_status_values[i]);
                            UART_DBG_UartPutString(message);
                        }
                        UART_DBG_UartPutString("\r\n");
                    }
                    else if (ch == 'R')
                    {
                        UART_ZING_PutChar(0x4);
                        UART_ZING_PutChar('r');
                        UART_ZING_PutChar('s');
                        UART_ZING_PutChar('t');
                        
                        if (rst == 0)
                        {
                            rst = 1;
                        }
                    }
                    else if (ch == 'C')
                    {
                        if (set_channel == 0)
                        {
                            set_channel = 1;
                        }
                    }
                    */
                }
#endif
            }
        }
    }
}

