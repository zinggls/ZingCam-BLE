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

#define ASCII_HOST 'Z'
#define ASCII_DEVICE 'Z'
#define ASCII_LF '\n'
#define MAX_BUFFER_LENGTH 256

static uint16_t uart_loop = 0;
static char zing_status[MAX_BUFFER_LENGTH];

CY_ISR(UART_ZING_RX_INTERRUPT)
{   
    char ch;
    uint16_t cnt;
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    uart_loop = uart_loop + 1;
    
    if (ch == ASCII_HOST || ch == ASCII_DEVICE)
    {
        zing_status[cnt++] = ch;
        
        do
        {
            if ((ch = UART_ZING_GetChar()) != 0)
            {
                if (cnt < MAX_BUFFER_LENGTH)
                {
                    zing_status[cnt++] = ch;
                }
                else
                {
                    return;
                }
            }
        }
        while (ch != ASCII_LF);
    }
    
    // Clear the interrupt to prevent it from retriggering
    UART_ZING_RX_ClearInterrupt();
    return;
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_DBG_Start();
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);

    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
