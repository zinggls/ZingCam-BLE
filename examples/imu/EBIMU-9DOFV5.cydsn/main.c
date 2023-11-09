#include <stdio.h>

#include "project.h"

int main(void)
{
    char msg[128];
    uint16_t SOP;
    
    CyGlobalIntEnable;
    
    UART_DBG_Start();
    UART_IMU_Start();
    
    UART_IMU_UartPutString("<soc2>");
    
    while (1)
    {
        CyDelay(1000);
    }
}