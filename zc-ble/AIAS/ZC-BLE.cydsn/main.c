#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"
#include "ble.h"

#if CYBLE_GAP_ROLE_CENTRAL
extern uint8_t** zing_device_status_values;
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
extern uint8_t** zing_host_status_values;
#endif
extern ZCBLE_frame zcble_frame;

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES + 2];
#if CYBLE_GAP_ROLE_CENTRAL
    uint8_t** zing_host_status_values;
    char zing_host_status[MAX_BUFFER_LENGTH];
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
    char message[128];
    uint8_t** zing_device_status_values;
    char zing_device_status[MAX_BUFFER_LENGTH];
    uint8_t rst;
    uint8_t set_channel;
#endif
    ZCBLE_frame zcble_frame;
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    char ch;
    
    UART_DBG_Start();
    
    ZCBLE_init();
    IMU_init();
    
#if CYBLE_GAP_ROLE_CENTRAL
    zing_host_status_values = ZING_host_init();
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
    zing_device_status_values = ZING_device_init();
    rst = 0;
    set_channel = 0;
#endif
    
    while (1)
    {
        CyBle_ProcessEvents();
        
        if (cyBle_state == CYBLE_STATE_CONNECTED)
        {
            if (CyBle_GattGetBusStatus() == CYBLE_STACK_STATE_FREE)
            {
                memset(&zcble_frame, 0, sizeof(ZCBLE_frame));
#if CYBLE_GAP_ROLE_CENTRAL
                if (ZING_get_host_status(zing_host_status) == 1)
                {
                    if (ZING_parse_host_status(zing_host_status, zing_host_status_values) != 1)
                    {
                        UART_DBG_UartPutString("Parse zing status failed\r\n");
                        continue;
                    }
                    else
                    {
                        for (uint8_t cnt = 0; cnt < NUM_HOST_STATUS; cnt++)
                        {
                            for (uint8_t idx = 0; idx < MAX_DATA_LENGTH; idx++)
                            {
                                zcble_frame.status_values[cnt * MAX_DATA_LENGTH + idx] = zing_host_status_values[cnt][idx];
                            }
                        }
                    }
                    
                    if (IMU_get(imu_values) != 1)
                    {
                        UART_DBG_UartPutString("Get IMU datas failed\r\n");
                        continue;
                    }
                    else
                    {
                        memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
                    }
                    
                    ZING_change_channel(zing_host_status_values, 0);
                    
                    notification.attrHandle = 0x0001;
                    notification.value.val = (uint8_t*)&zcble_frame;
                    notification.value.len = sizeof(ZCBLE_frame);
                    CyBle_GattsNotification(cyBle_connHandle, &notification);
                }
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
                if (ZING_get_device_status(zing_device_status) == 1)
                {
                    if (ZING_parse_device_status(zing_device_status, zing_device_status_values) != 1)
                    {
                        //UART_DBG_UartPutString("Parse zing status failed\r\n");
                        continue;
                    }
                    else
                    {
                        for (uint8_t cnt = 0; cnt < NUM_HOST_STATUS; cnt++)
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
                        continue;
                    }
                    else
                    {
                        memcpy(zcble_frame.imu_values, imu_values, sizeof(uint16_t) * NUM_TOTAL_IMU_VALUES);
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
                    
                    if (strcmp((char*)zing_device_status_values[DEVICE_STATUS_ITF], "Y") == 0)
                    {
//                      UART_ZING_PutChar(0x4);
//                      UART_ZING_PutChar('r');
//                      UART_ZING_PutChar('s');
//                      UART_ZING_PutChar('t');

                        set_channel = 1;
                    }
                    
                    if (set_channel == 1)
                    {
                        set_channel = 0;
                        zcble_frame.zing_params.set_channel = 1;
                    }
                    else
                    {
                        zcble_frame.zing_params.set_channel = 0;
                    }
                    
                    notification.attrHandle = 0x0001;
                    notification.value.val = (uint8_t*)&zcble_frame;
                    notification.value.len = sizeof(ZCBLE_frame);
                    CyBle_GattsNotification(cyBle_connHandle, &notification);
                    
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
//                      UART_ZING_PutChar(0x4);
//                      UART_ZING_PutChar('r');
//                      UART_ZING_PutChar('s');
//                      UART_ZING_PutChar('t');
                        
                        if (set_channel == 0)
                        {
                            set_channel = 1;
                        }
                    }
                }
#endif
            }
        }
    }
}

