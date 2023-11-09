#include <stdio.h>

#include "project.h"

int main(void)
{
    char msg[128];
    uint16_t SOP;
    uint16_t CHK;
    uint16_t DATA;
    uint8_t upper;
    uint8_t lower;
    
    CyGlobalIntEnable;
    
    UART_DBG_Start();
    UART_IMU_Start();
    
    UART_IMU_UartPutString("<soc2>");
    
    while (1)
    {
        UART_IMU_UartPutChar('*');
        
        upper = UART_IMU_UartGetChar();
        lower = UART_IMU_UartGetChar();
        SOP = upper << 8 | lower;
        CHK = 0x55 + 0x55;
        
        if (SOP == 0x5555)
        {
            UART_DBG_PutString("Receive SOP\r\n");
            
            while (1)
            {
                upper = UART_IMU_UartGetChar();
                lower = UART_IMU_UartGetChar();
                DATA = upper << 8 | lower;
                if (DATA != CHK)
                {
                    CHK = CHK + upper + lower;
                    sprintf(msg, "Receive %X (%X)\r\n", DATA, CHK);
                    UART_DBG_PutString(msg);
                    CyDelay(1000);
                }
                else
                {
                    UART_DBG_PutString("Receive CHK\r\n");
                    break;
                }                
            }
        }
        CyDelay(1000);
    }
}