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
    uint8_t lower,upper;
    uint16_t word;
    char msg[128];

    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_DBG_Start();
    UART_IMU_Start();

    UART_IMU_UartPutString("<soc2>");

    for(;;)
    {
        /* Place your application code here. */
        upper = UART_IMU_UartGetChar();
        lower = UART_IMU_UartGetChar();
        word = upper <<8 | lower;

        sprintf(msg, "%X\r\n", word);
        UART_DBG_UartPutString(msg);
        CyDelay(1000);
    }
}

/* [] END OF FILE */
