#include "ble.h"

void ble_callback(uint32_t event, void* parameters)
{
    switch (event)
    {
        case CYBLE_EVT_STACK_ON:
        break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
        break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
        break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
        break;
        case CYBLE_EVT_GATT_CONNECT_IND:
        break;
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
        break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
        break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
        break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
        break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
        break;
        case CYBLE_EVT_GATTC_WRITE_RSP:
        break;
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
        break;
    }
}