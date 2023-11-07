#include "project.h"
#include <stdio.h>

int main(void)
{
    uint8_t cnt;
    
    CyGlobalIntEnable;
    UART_DBG_Start();
    
    cnt = 0;
    
    while (1)
    {
        printf("Running cnt=%d\r\n", cnt);
        cnt = (cnt + 1) % 0xFF;
        
        CyDelay(1000);
    }
}

int _write(int file, char* ptr, int len)
{
    int i;
    file = file;
    for (i = 0; i < len; i++)
    {
        UART_DBG_UartPutChar(*ptr++);
    }
    return len;
}
