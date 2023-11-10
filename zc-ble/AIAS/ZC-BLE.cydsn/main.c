#include <stdlib.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

int main(void)
{
    CyGlobalIntEnable;
    
    uint16_t datas[NUM_TOTAL_IMU_DATAS];
    char zing_device_status[128];
    
    UART_DBG_Start();
    
    //IMU_init();
    ZING_init();
    
    while (1)
    {
        if (ZING_get_device_status(zing_device_status) == 1)
        {
        }
    }
}

