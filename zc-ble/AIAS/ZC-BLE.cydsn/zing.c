#include "zing.h"

void ZING_init(void)
{
    UART_ZING_Start();
}

uint8_t ZING_get_device_status(char* device_status)
{
    const char D = 'D';
    const char LF = '\n';
    char ch;
    uint16_t cnt;
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    
    if (ch == D)
    {
        device_status[cnt++] = D;
        
        do
        {
            if ((ch = UART_ZING_GetChar()) != 0)
            {
                device_status[cnt++] = ch;
            }
        }
        while (ch != LF);
        
        return 1;
    }
    else
    {
        return 0;
    }
}
