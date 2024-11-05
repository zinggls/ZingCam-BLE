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
static CYBLE_GAP_BD_ADDR_T addr;
static CYBLE_GAPC_ADV_REPORT_T advertisement_report;
static const uint8_t peripheral_addr[CYBLE_GAP_BD_ADDR_SIZE] = { 0x01, 0x00, 0x00, 0x50, 0xA0, 0x00 };

void ble_callback(uint32 evt, void* param);
void ble_events(uint32 evt, void* param);

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    CyBle_Start(ble_callback);
    UART_DBG_Start();

    UART_DBG_UartPutString("ble-central/client Start\r\n");
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
    uint32_t res;
    
    switch (evt)
    {
        // callback when stack is available
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            UART_DBG_UartPutString("CYBLE_EVT_STACK_ON\r\n");
            break;
        // callback when ble is disconnected
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            res = CyBle_GapRemoveDeviceFromWhiteList(&addr);
            sprintf(msg, "GAP REMOVE BONDED DEVICE RET=%X\n", res);
            UART_DBG_UartPutString(msg);
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            UART_DBG_UartPutString("CYBLE_EVT_GAP_DEVICE_DISCONNECTED\r\n");
            break;
        // callback when scanning is progressing
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            memcpy(&advertisement_report, param, sizeof(CYBLE_GAPC_ADV_REPORT_T));
            
            if (memcmp(peripheral_addr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE) == 0)
            {
                CyBle_GapcStopScan();
                addr.type = advertisement_report.peerAddrType;
                memcpy(addr.bdAddr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
            }
            UART_DBG_UartPutString("CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT\r\n");
            break;
        // callback when scanning is done
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if (cyBle_state == CYBLE_STATE_DISCONNECTED)
            {
                CyBle_GapcConnectDevice(&addr);
            }
            UART_DBG_UartPutString("CYBLE_EVT_GAPC_SCAN_START_STOP\r\n");
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            sprintf(msg, "AUTH FAILED=%X\n", *(uint32_t*)param);
            UART_DBG_UartPutString(msg);
            UART_DBG_UartPutString("CYBLE_EVT_GAP_AUTH_FAILED\r\n");
            break;
        // callback when ble is connected in application layer
        case CYBLE_EVT_GATT_CONNECT_IND:
            UART_DBG_UartPutString("CYBLE_EVT_GATT_CONNECT_IND\r\n");
            break;
        // callback when ble is connected in link layer
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
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
