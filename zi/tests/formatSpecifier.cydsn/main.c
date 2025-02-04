/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    char msg[128];
    UART_DBG_Start();

    int nCount = 0;
    for(;;)
    {
        /* Place your application code here. */
        UART_DBG_UartPutString("Hello Uart\r\n");
        
        sprintf(msg, "format specifier test=%d\n", nCount++);
        UART_DBG_UartPutString(msg);
        CyDelay(1000);
    }
}

/* [] END OF FILE */
