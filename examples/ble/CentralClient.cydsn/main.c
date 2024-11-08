#include <stdio.h>
#include "project.h"

static char msg[128];
static CYBLE_GAPC_ADV_REPORT_T advertisement_report;
static const uint8_t peripheral_addr[CYBLE_GAP_BD_ADDR_SIZE] = { 0x01, 0x00, 0x00, 0x50, 0xA0, 0x00 };
static CYBLE_GAP_BD_ADDR_T addr;
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
    
    sprintf(msg, "BLE state = %s %X\r\n", state, evt);
    UART_DBG_PutString(msg);
    
    uint32_t res;
    
    switch (evt)
    {
        // callback when stack is available
        case CYBLE_EVT_STACK_ON:
            // start scanning when ble stack is available
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;
        // callback when ble is disconnected
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            // start scanning when ble disconnected
            //res = CyBle_GapRemoveBondedDevice(&addr);
            res = CyBle_GapRemoveDeviceFromWhiteList(&addr);
            sprintf(msg, "GAP REMOVE BONDED DEVICE RET=%X\n", res);
            UART_DBG_PutString(msg);
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;
        // callback when scanning is progressing
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            // receive device's information in param
            // typecast param to CYBLE_GAPC_ADV_REPORT_T
            memcpy(&advertisement_report, param, sizeof(CYBLE_GAPC_ADV_REPORT_T));
            
            if (memcmp(peripheral_addr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE) == 0)
            {
                CyBle_GapcStopScan();
                addr.type = advertisement_report.peerAddrType;
                memcpy(addr.bdAddr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
            }
            break;
        // callback when scanning is done
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if (cyBle_state == CYBLE_STATE_DISCONNECTED)
            {
                CyBle_GapcConnectDevice(&addr);
            }
            break;
        case CYBLE_EVT_GAP_AUTH_FAILED:
            sprintf(msg, "AUTH FAILED=%X\n", *(uint32_t*)param);
            UART_DBG_PutString(msg);
            break;
        // callback when ble is connected in application layer
        case CYBLE_EVT_GATT_CONNECT_IND:
            break;
        // callback when ble is connected in link layer
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
            break;
        // callback when ble is authenticated in link layer
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            // start ble server discovery process
            CyBle_GattcStartDiscovery(cyBle_connHandle);
            break;            
        // callback when ble is discovered device
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            break;
        // callback when receive mtu request
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            CyBle_GattsExchangeMtuRsp(cyBle_connHandle, 200);
            UART_DBG_PutString("MTU REQ\r\n");
            break;
        // callback when receive mtu response
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
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
