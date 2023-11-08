#include <stdio.h>
#include "project.h"

static char msg[128];

void ble_callback(uint32 evt, void* param);

int main(void)
{
    CyGlobalIntEnable;
    
    CyBle_Start(ble_callback);
    UART_DBG_Start();

    while (1)
    {
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
    UART_DBG_PutString(msg);
    
    switch (evt)
    {
        case CYBLE_EVT_STACK_ON:
        // callback when stack is available
        break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
        // callback when ble is disconnected
        break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        // callback when scannin is progressing
        break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
        // callback when scanning is done
        break;
        case CYBLE_EVT_GATT_CONNECT_IND:
        // callback when ble is connected in application layer
        break;
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
        // callback when ble is connected in link layer
        break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
        // callback when ble is authenticated in link layer
        break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
        // callback when ble is discovered device
        break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        // callback when receive mtu request
        break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
        // callback when receive mtu response
        break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
        // callback when receive write request
        break;
        case CYBLE_EVT_GATTC_WRITE_RSP:
        // callback when receive write response
        break;
    }
}