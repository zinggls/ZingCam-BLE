#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t imu_datas[NUM_TOTAL_IMU_DATAS];
    char message[128];
#if CYBLE_GAP_ROLE_CENTRAL
    uint8_t** zing_host_status_values;
    char zing_host_status[MAX_BUFFER_LENGTH];
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
    uint8_t** zing_device_status_values;
    char zing_device_status[MAX_BUFFER_LENGTH];
#endif
    
    UART_DBG_Start();
    
    IMU_init();
#if CYBLE_GAP_ROLE_CENTRAL
    zing_host_status_values = ZING_host_init();
#endif
#if CYBLE_GAP_ROLE_PERIPHERAL
    zing_device_status_values = ZING_device_init();
#endif
    
    while (1)
    {
        if (ZING_get_host_status(zing_host_status) == 1)
        {
            ZING_parse_host_status(zing_host_status, zing_host_status_values);
            
            for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
            {
                sprintf(message, "%s\r\n", zing_host_status_values[i]);
                UART_DBG_UartPutString(message);
            }
        }
    }
}

