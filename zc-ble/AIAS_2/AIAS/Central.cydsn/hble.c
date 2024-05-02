#include "hble.h"
#include "../common/headers/zing.h"

static const uint8_t peripheral_address[CYBLE_GAP_BD_ADDR_SIZE] = { 0x01, 0x00, 0x00, 0x50, 0xA0, 0x00 };
static CYBLE_GAPC_ADV_REPORT_T advertisement_report;
static CYBLE_GAP_BD_ADDR_T device_address;
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T notification;

static void HBLE_callback(unsigned long event, void* parameters);

void HBLE_init(void)
{
    CyBle_Start(HBLE_callback);
}

static void HBLE_callback(unsigned long event, void* parameters)
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
    
    switch (event)
    {
        case CYBLE_EVT_STACK_ON:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;
        case CYBLE_EVT_GAP_DEVICE_CONNECTED:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            CyBle_GapcStartScan(CYBLE_SCANNING_FAST);
            break;
        case CYBLE_EVT_GAPC_SCAN_PROGRESS_RESULT:
            memcpy(&advertisement_report, parameters, sizeof(CYBLE_GAPC_ADV_REPORT_T));
            
            if (memcmp(peripheral_address, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE) == 0)
            {
                CyBle_GapcStopScan();
                device_address.type = advertisement_report.peerAddrType;
                memcpy(device_address.bdAddr, advertisement_report.peerBdAddr, CYBLE_GAP_BD_ADDR_SIZE);
            }
            break;
        case CYBLE_EVT_GAPC_SCAN_START_STOP:
            if (cyBle_state == CYBLE_STATE_DISCONNECTED)
            {
                CyBle_GapcConnectDevice(&device_address);
            }
            break;
        case CYBLE_EVT_GAP_ENHANCE_CONN_COMPLETE:
            CyBle_GapAuthReq(cyBle_connHandle.bdHandle, &cyBle_authInfo);
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
            
            if (ble_frame.type == -1)
            {
                break;
            }
            
            sprintf(msg, "USB:%d ", ble_frame.zing_frame.usb);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PPID:0x%X ", ble_frame.zing_frame.ppid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "DeviceID:0x%X ", ble_frame.zing_frame.device_id);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "FMT:%d ", ble_frame.zing_frame.fmt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "IDX:%d ", ble_frame.zing_frame.idx);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "FPS:0x%X ", ble_frame.zing_frame.fps);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "TRT:%d ", ble_frame.zing_frame.trt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "ACK:%d ", ble_frame.zing_frame.ack);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PPC:%d ", ble_frame.zing_frame.ppc);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "RUN:%d ", ble_frame.zing_frame.run);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "ITF:%d ", ble_frame.zing_frame.itf);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "TXID:0x%X ", ble_frame.zing_frame.txid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "RXID:0x%X ", ble_frame.zing_frame.rxid);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "DestID_ERR_CNT:%d ", ble_frame.zing_frame.destid_err_cnt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "PHY_RX_FRAME_CNT:%d ", ble_frame.zing_frame.phy_rx_frame_cnt);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "MFIR:%d ", ble_frame.zing_frame.mfir);
            UART_DBG_UartPutString(msg);
            sprintf(msg, "CNT:%d\r\n", ble_frame.zing_frame.cnt);
            UART_DBG_UartPutString(msg);
            
            break;
    }
}

AIAS_BLE_FRAME HBLE_set_frame(char* zing_status, char** status_values)
{
    AIAS_BLE_FRAME ble_frame;
    
    memset(&ble_frame, 0, sizeof(AIAS_BLE_FRAME));
    
    if (ZING_parse_zch_status(zing_status, status_values) != NUM_ZCH_STATUS)
    {
        ble_frame.type = -1;
    }
    else
    {     
        ble_frame.type = 0;
        
        ble_frame.zing_frame.usb = ZING_parse_status_values(status_values[ZCH_USB], ZCH_USB);
        ble_frame.zing_frame.vnd = ZING_parse_status_values(status_values[ZCH_VND], ZCH_VND);
        ble_frame.zing_frame.prd = ZING_parse_status_values(status_values[ZCH_PRD], ZCH_PRD);
        ble_frame.zing_frame.ppid = ZING_parse_status_values(status_values[ZCH_PPID], ZCH_PPID);
        ble_frame.zing_frame.device_id = ZING_parse_status_values(status_values[ZCH_DEVICE_ID], ZCH_DEVICE_ID);
        ble_frame.zing_frame.fmt = ZING_parse_status_values(status_values[ZCH_FMT], ZCH_FMT);
        ble_frame.zing_frame.idx = ZING_parse_status_values(status_values[ZCH_IDX], ZCH_IDX);
        ble_frame.zing_frame.trt = ZING_parse_status_values(status_values[ZCH_TRT], ZCH_TRT);
        ble_frame.zing_frame.ack = ZING_parse_status_values(status_values[ZCH_ACK], ZCH_ACK);
        ble_frame.zing_frame.ppc = ZING_parse_status_values(status_values[ZCH_PPC], ZCH_PPC);
        ble_frame.zing_frame.txid = ZING_parse_status_values(status_values[ZCH_TXID], ZCH_TXID);
        ble_frame.zing_frame.rxid = ZING_parse_status_values(status_values[ZCH_RXID], ZCH_RXID);
        ble_frame.zing_frame.run = ZING_parse_status_values(status_values[ZCH_RUN], ZCH_RUN);
        ble_frame.zing_frame.cnt = ZING_parse_status_values(status_values[ZCH_CNT], ZCH_CNT);
    }
    
    return ble_frame;
}