#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t datas[NUM_TOTAL_IMU_DATAS];
    char zing_device_status[128];
    char message[128];
    uint8_t** zing_device_status_values;
    
    UART_DBG_Start();
    
    //IMU_init();
    ZING_init(zing_device_status_values);
    
    while (1)
    {
        if (ZING_get_device_status(zing_device_status) == 1)
        {
            ZING_parse_device_status(zing_device_status, (uint8_t**)zing_device_status_values);
            
            for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
            {
                sprintf(message, "%s\r\n", zing_device_status_values[i]);
                UART_DBG_UartPutString(message);
            }
        }
    }
}

