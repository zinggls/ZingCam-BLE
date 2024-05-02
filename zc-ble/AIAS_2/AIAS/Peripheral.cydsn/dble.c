#include "dble.h"
#include "../common/headers/zing.h"

static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T notification;

static void DBLE_callback(unsigned long event, void* parameters);

void DBLE_init(void)
{
    CyBle_Start(DBLE_callback);
}

static void DBLE_callback(unsigned long event, void* parameters)
{
    char state[13];
    char msg[128];
    AIAS_BLE_FRAME ble_frame;

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

    switch (event)
    {
        case CYBLE_EVT_STACK_ON:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
            break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            CyBle_GattcStartDiscovery(cyBle_connHandle);
            break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            CyBle_GattcExchangeMtuReq(cyBle_connHandle, MAX_BLE_FRAME_SIZE);
            break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            CyBle_GattsWriteRsp(cyBle_connHandle);
            break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
            break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
            break;
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            memcpy(&notification, parameters, sizeof(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T));
            memcpy(&ble_frame, notification.handleValPair.value.val, notification.handleValPair.value.len);
        
            sprintf(msg, "USB:%d ", ble_frame.zing_frame.usb);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "VND:0x%X ", ble_frame.zing_frame.vnd);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PRD:0x%X ", ble_frame.zing_frame.prd);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "BND:0x%X ", ble_frame.zing_frame.bnd);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PPID:0x%X ", ble_frame.zing_frame.ppid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "DeviceID:0x%X ", ble_frame.zing_frame.device_id);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "FMT:%d ", ble_frame.zing_frame.fmt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "IDX:%d ", ble_frame.zing_frame.idx);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "TRT:%d ", ble_frame.zing_frame.trt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "ACK:%d ", ble_frame.zing_frame.ack);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PPC:%d ", ble_frame.zing_frame.ppc);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "TXID:0x%X ", ble_frame.zing_frame.txid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "RXID:0x%X ", ble_frame.zing_frame.rxid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "RUN:%d ", ble_frame.zing_frame.run);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "CNT:%d\r\n", ble_frame.zing_frame.cnt);
            UART_DBG_UartPutString(msg);
            
            break;
    }
}

AIAS_BLE_FRAME DBLE_set_frame(char* zing_status, char** status_values)
{
    AIAS_BLE_FRAME ble_frame;
    
    memset(&ble_frame, 0, sizeof(AIAS_BLE_FRAME));
    
    if (ZING_parse_zcd_status(zing_status, status_values) == 0)
    {
        ble_frame.type = -1;
    }
    else
    {     
        ble_frame.type = 0;
        
        ble_frame.zing_frame.usb = ZING_parse_status_values(status_values[ZCD_USB], ZCD_USB);
        ble_frame.zing_frame.ppid = ZING_parse_status_values(status_values[ZCD_PPID], ZCD_PPID);
        ble_frame.zing_frame.device_id = ZING_parse_status_values(status_values[ZCD_DEVICE_ID], ZCD_DEVICE_ID);
        ble_frame.zing_frame.fmt = ZING_parse_status_values(status_values[ZCD_FMT], ZCD_FMT);
        ble_frame.zing_frame.idx = ZING_parse_status_values(status_values[ZCD_IDX], ZCD_IDX);
        ble_frame.zing_frame.fps = ZING_parse_status_values(status_values[ZCD_FPS], ZCD_FPS);
        ble_frame.zing_frame.trt = ZING_parse_status_values(status_values[ZCD_TRT], ZCD_TRT);
        ble_frame.zing_frame.ack = ZING_parse_status_values(status_values[ZCD_ACK], ZCD_ACK);
        ble_frame.zing_frame.ppc = ZING_parse_status_values(status_values[ZCD_PPC], ZCD_PPC);
        ble_frame.zing_frame.run = ZING_parse_status_values(status_values[ZCD_RUN], ZCD_RUN);
        ble_frame.zing_frame.itf = ZING_parse_status_values(status_values[ZCD_ITF], ZCD_ITF);
        ble_frame.zing_frame.txid = ZING_parse_status_values(status_values[ZCD_TXID], ZCD_TXID);
        ble_frame.zing_frame.rxid = ZING_parse_status_values(status_values[ZCD_RXID], ZCD_RXID);
        ble_frame.zing_frame.destid_err_cnt = ZING_parse_status_values(status_values[ZCD_DESTID_ERR_CNT], ZCD_DESTID_ERR_CNT);
        ble_frame.zing_frame.phy_rx_frame_cnt = ZING_parse_status_values(status_values[ZCD_PHY_RX_FRAME_CNT], ZCD_PHY_RX_FRAME_CNT);
        ble_frame.zing_frame.mfir = ZING_parse_status_values(status_values[ZCD_MFIR], ZCD_MFIR);
        ble_frame.zing_frame.cnt = ZING_parse_status_values(status_values[ZCD_CNT], ZCD_CNT);
    }
    
    return ble_frame;
}