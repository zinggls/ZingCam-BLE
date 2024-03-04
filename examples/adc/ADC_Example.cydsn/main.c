#include <stdio.h>

#include "project.h"

int main(void)
{
    uint16_t result;
    char msg[128];
    int cnt;
    
    UART_DBG_Start();
    ADC_Start();
    
    P2_6_Write(0);
    cnt = 0;
    
    while (1)
    {
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result = ADC_GetResult16(0);
        sprintf(msg, "%d. res=%d\r\n", cnt++, result);
        UART_DBG_PutString(msg);
        CyDelay(1000);
    }
}