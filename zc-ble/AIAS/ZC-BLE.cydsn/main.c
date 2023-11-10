#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t datas[NUM_TOTAL_IMU_DATAS];
    char zing_host_status[MAX_BUFFER_LENGTH];
    char zing_device_status[MAX_BUFFER_LENGTH];
    char message[128];
    uint8_t** zing_host_status_values;
    uint8_t** zing_device_status_values;
    
    UART_DBG_Start();
    
    //IMU_init();
    zing_host_status_values = ZING_host_init();
    
    while (1)
    {
        if (ZING_get_host_status(zing_host_status) == 1)
        {
            UART_DBG_UartPutString(message);
            ZING_parse_host_status(zing_host_status, zing_host_status_values);
            
            for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
            {
                sprintf(message, "%s\r\n", zing_host_status_values[i]);
                UART_DBG_UartPutString(message);
            }
        }
    }
}

