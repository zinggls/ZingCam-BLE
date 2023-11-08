#include <stdio.h>

#include "project.h"

int main(void)
{
    char ch;
    
    CyGlobalIntEnable;
    
    UART_DBG_Start();
    UART_IMU_Start();
    
    while (1)
    {
        UART_IMU_UartPutChar('*');
        while (1)
        {
            ch = UART_IMU_UartGetChar();
            UART_DBG_PutChar(ch);
            
            if (ch == '\n')
            {
                break;
            }
        }
        CyDelay(1000);
    }
}
