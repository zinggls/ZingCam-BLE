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

void ble_callback(uint32 evt, void* param);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBle_Start(ble_callback);
    UART_DBG_Start();

    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        CyBle_ProcessEvents();
    }
}

void ble_callback(uint32 evt, void* param)
{
    UART_DBG_UartPutString("ble_callback\r\n");
}

/* [] END OF FILE */
