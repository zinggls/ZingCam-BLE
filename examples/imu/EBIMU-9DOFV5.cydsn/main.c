#include <stdio.h>

#include "project.h"

int main(void)
{
    char msg[128];
    uint8_t upper;
    uint8_t lower;
    uint16_t SOP;
    uint16_t CHK;
    uint16_t DATA;
    uint16_t datas[7];
    uint8_t cnt = 0;
    
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
        
        if (SOP == 0x5555)
        {
            //UART_DBG_PutString("Receive SOP\r\n");
            cnt = 0;
            CHK = 0x55 + 0x55;
            
            while (1)
            {
                upper = UART_IMU_UartGetChar();
                lower = UART_IMU_UartGetChar();
                DATA = upper << 8 | lower;
                if (DATA != CHK)
                {
                    CHK = CHK + upper + lower;
                    datas[cnt] = DATA;
                    cnt = cnt + 1;
                    //sprintf(msg, "Receive %X (%X)\r\n", DATA, CHK);
                    //UART_DBG_PutString(msg);
                }
                else
                {
                    //UART_DBG_PutString("Receive CHK\r\n");
                    break;
                }                
            }
        }
        
        for (uint8_t i = 0; i < 7; i++)
        {
            sprintf(msg, "%X ", datas[i]);
            UART_DBG_PutString(msg);
        }
        UART_DBG_PutString("\r\n");
        
        CyDelay(1000);
    }
}