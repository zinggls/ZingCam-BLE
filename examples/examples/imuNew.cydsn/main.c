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
#include <UartBuf.h>

static UartBuf uBuf;    //Circular buffer for UART data

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    UartBuf_write_char(&uBuf,UART_IMU_GetChar());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
}

// Function to process data when a complete message is available
static void process_uart_data()
{
    while (!UartBuf_is_empty(&uBuf)) {
        UART_DBG_UartPutChar(UartBuf_read_char(&uBuf));
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UartBuf_init(&uBuf);
    
    UART_DBG_Start();
    UART_IMU_Start();
    
    UART_IMU_RX_INTR_StartEx(UART_IMU_RX_INTERRUPT);    
    
    UART_IMU_PutString("<lf>");
    CyDelay(100);
    UART_IMU_PutString("<sor1>");
    CyDelay(100);
    UART_IMU_PutString("<soc2>");
    CyDelay(100);
    UART_IMU_PutString("<sots1>");
    CyDelay(100);
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        process_uart_data();
    }
}

/* [] END OF FILE */
