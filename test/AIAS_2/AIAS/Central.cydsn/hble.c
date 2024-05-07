#include "hble.h"
#include "../common/headers/imu.h"
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

AIAS_ZING_FRAME HBLE_set_zing_frame(char* zing_status, char** status_values)
{
    AIAS_ZING_FRAME zing_frame;
    char status_value[MAX_STATUS_VALUE_LENGTH];
    
    memset(&zing_frame, 0, sizeof(AIAS_ZING_FRAME));
    
    if (ZING_parse_zch_status(zing_status, status_values) != NUM_ZCH_STATUS)
    {
    }
    else
    {   
        strcpy(status_value, status_values[ZCH_USB]);
        zing_frame.usb = ZING_parse_zch_status_values(status_value, ZCH_USB);
        strcpy(status_value, status_values[ZCH_VND]);
        zing_frame.vnd = ZING_parse_zch_status_values(status_value, ZCH_VND);
        strcpy(status_value, status_values[ZCH_PRD]);
        zing_frame.prd = ZING_parse_zch_status_values(status_value, ZCH_PRD);
        strcpy(status_value, status_values[ZCH_PPID]);
        zing_frame.ppid = ZING_parse_zch_status_values(status_value, ZCH_PPID);
        strcpy(status_value, status_values[ZCH_DEVICE_ID]);
        zing_frame.device_id = ZING_parse_zch_status_values(status_value, ZCH_DEVICE_ID);
        strcpy(status_value, status_values[ZCH_FMT]);
        zing_frame.fmt = ZING_parse_zch_status_values(status_value, ZCH_FMT);
        strcpy(status_value, status_values[ZCH_IDX]);
        zing_frame.idx = ZING_parse_zch_status_values(status_value, ZCH_IDX);
        strcpy(status_value, status_values[ZCH_TRT]);
        zing_frame.trt = ZING_parse_zch_status_values(status_value, ZCH_TRT);
        strcpy(status_value, status_values[ZCH_ACK]);
        zing_frame.ack = ZING_parse_zch_status_values(status_value, ZCH_ACK);
        strcpy(status_value, status_values[ZCH_PPC]);
        zing_frame.ppc = ZING_parse_zch_status_values(status_value, ZCH_PPC);
        strcpy(status_value, status_values[ZCH_TXID]);
        zing_frame.txid = ZING_parse_zch_status_values(status_value, ZCH_TXID);
        strcpy(status_value, status_values[ZCH_RXID]);
        zing_frame.rxid = ZING_parse_zch_status_values(status_value, ZCH_RXID);
        strcpy(status_value, status_values[ZCH_RUN]);
        zing_frame.run = ZING_parse_zch_status_values(status_value, ZCH_RUN);
        strcpy(status_value, status_values[ZCH_CNT]);
        zing_frame.cnt = ZING_parse_zch_status_values(status_value, ZCH_CNT);
    }
    
    return zing_frame;
}

AIAS_IMU_FRAME HBLE_set_imu_frame(char* imu, char** imu_values, uint8_t type)
{
    AIAS_IMU_FRAME imu_frame;
    char imu_value[MAX_IMU_VALUE_LENGTH];
    
    memset(&imu_frame, 0, sizeof(AIAS_IMU_FRAME));
    
    if (IMU_parse(imu, imu_values, 0) != 1)
    {
    }
    else
    {
        if (type == 0)
        {
            strcpy(imu_value, imu_values[IMU_R_Z]);
            imu_frame.r_z = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_P_Y]);
            imu_frame.p_y = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_Y_X]);
            imu_frame.y_x = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_TS_W]);
            imu_frame.timestamp = atoi(imu_value);
        }
        else
        {
            strcpy(imu_value, imu_values[IMU_R_Z]);
            imu_frame.r_z = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_P_Y]);
            imu_frame.p_y = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_Y_X]);
            imu_frame.y_x = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU_TS_W]);
            imu_frame.ts_w = IMU_parse_value(imu_value);
            strcpy(imu_value, imu_values[IMU___TS]);
            imu_frame.timestamp = atoi(imu_value);
        }
    }
    
    return imu_frame;
}