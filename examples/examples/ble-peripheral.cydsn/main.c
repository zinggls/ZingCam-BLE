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

    UART_DBG_UartPutString("ble-peripheral Start\r\n");
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
        case CYBLE_STATE_ADVERTISING:
            sprintf(state, "advertising");
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
    switch (evt)
    {
        // callback when stack is available
        case CYBLE_EVT_STACK_ON:
            UART_DBG_UartPutString("CYBLE_EVT_STACK_ON\r\n");
            break;
        // callback when ble is disconnected
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            UART_DBG_UartPutString("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;
        // callback when ble is connected in application layer
        case CYBLE_EVT_GATT_CONNECT_IND:
            UART_DBG_UartPutString("CYBLE_EVT_GATT_CONNECT_IND\r\n");
            break;
        // callback when ble is connected in link layer
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            UART_DBG_UartPutString("CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE\r\n");
            break;
        // callback when ble is authenticated in link layer
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            UART_DBG_UartPutString("CYBLE_EVT_GAP_AUTH_COMPLETE\r\n");
            break;
        // callback when ble is discovered device
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            UART_DBG_UartPutString("CYBLE_EVT_GATTC_DISCOVERY_COMPLETE\r\n");
            break;
        // callback when receive mtu request
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            UART_DBG_UartPutString("CYBLE_EVT_GATTS_XCNHG_MTU_REQ\r\n");
            break;
        // callback when receive mtu response
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            UART_DBG_UartPutString("CYBLE_EVT_GATTC_XCHNG_MTU_RSP\r\n");
            break;
        // callback when receive write request
        case CYBLE_EVT_GATTS_WRITE_REQ:
            UART_DBG_UartPutString("CYBLE_EVT_GATTS_WRITE_REQ\r\n");
            break;
        // callback when receive write response
        case CYBLE_EVT_GATTC_WRITE_RSP:
            UART_DBG_UartPutString("CYBLE_EVT_GATTC_WRITE_RSP\r\n");
            break;
        // callback when receive notification
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            UART_DBG_UartPutString("CYBLE_EVT_GATTC_HANDLE_VALUE_NTF\r\n");
            break;
    }
}

/* [] END OF FILE */
