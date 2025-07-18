#include <stdio.h>
#include "project.h"

static char msg[128];
static uint32_t sec;
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T recv;

void ble_callback(uint32 evt, void* param);
void systick_isr_callback(void);

int main(void)
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T notification;
    uint8_t data = 0;
    
    CyGlobalIntEnable;
    
    CySysTickStart();
    CyBle_Start(ble_callback);
    UART_DBG_Start();
    
    for (uint8 i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; i++)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            CySysTickSetCallback(i, systick_isr_callback);
            break;
        }
    }
    
    while (1)
    {
        CyBle_ProcessEvents();
        
        if (sec == 0)
        {
            if (cyBle_state == CYBLE_STATE_CONNECTED)
            {
                // alert notification
                notification.value.len = 1;
                notification.value.val = &data;
                CyBle_GattsNotification(cyBle_connHandle, &notification);
                
                data = (data + 1) % 0xFF;
                CyDelay(1);
            }
        }
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
    UART_DBG_PutString(msg);

    switch (evt)
    {
        // callback when stack is available
        case CYBLE_EVT_STACK_ON:
            // start advertising when ble stack is available
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        // callback when ble is disconnected
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            // start advertising when ble disconnected
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        // callback when ble is connected in application layer
        case CYBLE_EVT_GATT_CONNECT_IND:
            break;
        // callback when ble is connected in link layer
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            break;
        // callback when ble is discovered device
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            CyBle_GattcExchangeMtuReq(cyBle_connHandle, 200);
            break;
        // callback when receive mtu request
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            break;
        // callback when receive mtu response
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            UART_DBG_PutString("MTU RSP\r\n");
            break;
        // callback when receive write request
        case CYBLE_EVT_GATTS_WRITE_REQ:
            CyBle_GattsWriteRsp(cyBle_connHandle);
            break;
        // callback when receive write response
        case CYBLE_EVT_GATTC_WRITE_RSP:
            break;
        // callback when receive notification
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            memcpy(&recv, param, sizeof(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T));
            sprintf(msg, "receive data = %d, (rssi = %d)\r\n", *recv.handleValPair.value.val, CyBle_GetRssi());
            UART_DBG_PutString(msg);
            break;
    }
}

void systick_isr_callback(void)
{
    // Callback 1 sec
    sec = (sec + 1) % 1000;
}
