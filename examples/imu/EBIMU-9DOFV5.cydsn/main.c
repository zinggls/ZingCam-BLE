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
        UART_IMU_UartPutChar('*');
        
        SOP = UART_IMU_UartGetChar() << 8 | UART_IMU_UartGetChar();
        
        if (SOP == 0x5555)
        {
            UART_DBG_PutString("Receive SOP\r\n");
        }
        
        CyDelay(1000);
    }
}