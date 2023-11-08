#include "project.h"

int main(void)
{
    CyGlobalIntEnable;
    
    UART_DBG_Start();
    
    while (1)
    {
        UART_DBG_PutString("Hello World\r\n");
        CyDelay(1000);
    }
}
