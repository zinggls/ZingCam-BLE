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

static char msg[128];

void ble_callback(uint32 evt, void* param);
void ble_events(uint32 evt, void* param);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBle_Start(ble_callback);
    UART_DBG_Start();

    UART_DBG_UartPutString("ble-central Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        CyBle_ProcessEvents();
    }
}

void ble_callback(uint32 evt, void* param)
{
    char state[13];

    switch (cyBle_state)
    {
        case CYBLE_STATE_STOPPED:
            sprintf(state, "stopped");
            break;
        case CYBLE_STATE_INITIALIZING:
            sprintf(state, "initializing");
            break;
        case CYBLE_STATE_CONNECTED:
            sprintf(state, "connected");
            break;
        case CYBLE_STATE_SCANNING:
            sprintf(state, "scanning");
            break;
        case CYBLE_STATE_CONNECTING:
            sprintf(state, "connecting");
            break;
        case CYBLE_STATE_DISCONNECTED:
            sprintf(state, "disconnected");
            break;
    }
    sprintf(msg, "BLE state = %s\r\n", state);
    UART_DBG_UartPutString(msg);
    
    ble_events(evt,param);
}

void ble_events(uint32 evt, void* param)
{
    (void)evt;
    (void)param;
}

/* [] END OF FILE */
