#include <stdlib.h>

#include "ble.h"
#include "main.h"
#include "icd.h"
#include "zing.h"

#if HBLE
static const uint8_t peripheral_address[CYBLE_GAP_BD_ADDR_SIZE] = { 0x01, 0x00, 0x00, 0x50, 0xA0, 0x00 };
static CYBLE_GAPC_ADV_REPORT_T advertisement_report;
static CYBLE_GAP_BD_ADDR_T device_address;
#endif
static CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T notification;
ZCBLE_frame zcble_frame;
#if HBLE
static uint8_t** zing_device_status_values;
#endif
#if DBLE
static uint8_t** zing_host_status_values;
#endif

static void ZCBLE_callback(uint32_t event, void* parameters);

#if HBLE
uint8_t** ZCBLE_get_zing_device_status_values(void)
{
    return zing_device_status_values;
}
#endif

#if DBLE
uint8_t** ZCBLE_get_zing_host_status_values(void)
{
    return zing_host_status_values;
}
#endif

void ZCBLE_init(void)
{
#if HBLE
    zing_device_status_values = (uint8_t**)calloc(NUM_DEVICE_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
    {
        zing_device_status_values[i] = (uint8_t*)calloc(MAX_DATA_LENGTH, sizeof(uint8_t));
    }
#endif
#if DBLE
    zing_host_status_values = (uint8_t**)calloc(NUM_HOST_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
    {
        zing_host_status_values[i] = (uint8_t*)calloc(MAX_DATA_LENGTH, sizeof(uint8_t));
    }
#endif
    
    CyBle_Start(ZCBLE_callback);
}

void ZCBLE_callback(uint32_t event, void* parameters)
{
    switch (event)
    {
#if HBLE
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
            memcpy(&zcble_frame, notification.handleValPair.value.val, notification.handleValPair.value.len);
            
            if (zcble_frame.type == ZCBLE_UNKNOWN)
            {
                break;
            }
            
            if (zcble_frame.type == ZCBLE_WRITE)
            {
                AIAS_ICD_set_scope(zcble_frame.icd_params.scope);
                AIAS_ICD_set_wireless_channel(zcble_frame.icd_params.w_c);
                AIAS_ICD_set_transitter_imu(zcble_frame.icd_params.tx_imu);
                AIAS_ICD_set_opmode(zcble_frame.icd_params.opmode);
            }
            
            ZING_set_itf(zcble_frame.status.itf);
            
            //AIAS_ICD_update_device_status(zcble_frame.status, NULL);
        break;
#endif
#if DBLE
        case CYBLE_EVT_STACK_ON:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x00);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        case CYBLE_EVT_GAP_DEVICE_DISCONNECTED:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x00);
            CyBle_GappStartAdvertisement(CYBLE_ADVERTISING_FAST);
        break;
        case CYBLE_EVT_GAP_AUTH_COMPLETE:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattcStartDiscovery(cyBle_connHandle);
        break;
        case CYBLE_EVT_GATTC_DISCOVERY_COMPLETE:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattcExchangeMtuReq(cyBle_connHandle, MAX_BLE_FRAME_SIZE);
        break;
        case CYBLE_EVT_GATTS_WRITE_REQ:
            AIAS_ICD_set(AIAS_BLE_STATUS, 0x01);
            CyBle_GattsWriteRsp(cyBle_connHandle);
        break;
        case CYBLE_EVT_GATTS_XCNHG_MTU_REQ:
        break;
        case CYBLE_EVT_GATTC_XCHNG_MTU_RSP:
        break;
        case CYBLE_EVT_GATTC_HANDLE_VALUE_NTF:
            memcpy(&notification, parameters, sizeof(CYBLE_GATTC_HANDLE_VALUE_NTF_PARAM_T));
            memcpy(&zcble_frame, notification.handleValPair.value.val, notification.handleValPair.value.len);
            
            if (zcble_frame.type == ZCBLE_UNKNOWN)
            {
                break;
            }
            
            if (zcble_frame.type == ZCBLE_WRITE)
            {   
                AIAS_ICD_set_scope(zcble_frame.icd_params.scope);
                AIAS_ICD_set_wireless_channel(zcble_frame.icd_params.w_c);
                AIAS_ICD_set_opmode(zcble_frame.icd_params.opmode);
                AIAS_ICD_set_transitter_imu(zcble_frame.icd_params.tx_imu);
                AIAS_ICD_set_battery_level(zcble_frame.icd_params.battey);
                AIAS_ICD_set_modules_status(zcble_frame.icd_params.modules);
                AIAS_ICD_set_modem_status(zcble_frame.icd_params.modem);   
            }
            
            AIAS_ICD_set_transmitter_imu_data(IMU_EULER, zcble_frame.imu_values);
            AIAS_ICD_update_host_status(zcble_frame.status, NULL);
            
            //LED_USER_Write(!(LED_USER_Read()));  
            
        break;
#endif
    }
}

ZING_status ZING_zed_set_status(uint8_t** status_values)
{
    ZING_status status;
    
    ZING_get_status(status_values, 1, ZED_STATUS_USB, &status.usb);
    status.vendor_id = 0;
    status.product_id = 0;
    ZING_get_status(status_values, 0, ZED_STATUS_BND, &status.channel);
    ZING_get_status(status_values, 2, ZED_STATUS_PPID, (uint8_t*)&status.ppid);
    ZING_get_status(status_values, 2, ZED_STATUS_DEVID, (uint8_t*)&status.device_id);
    status.format = 0;
    status.index = 0;
    ZING_get_status(status_values, 0, ZED_STATUS_TRT, &status.trt);
    ZING_get_status(status_values, 0, ZED_STATUS_ACK, &status.ack);
    ZING_get_status(status_values, 0, ZED_STATUS_PPC, &status.ppc);
    ZING_get_status(status_values, 3, ZED_STATUS_TXID, (uint8_t*)&status.txid);
    ZING_get_status(status_values, 3, ZED_STATUS_RXID, (uint8_t*)&status.rxid);
    ZING_get_status(status_values, 0, ZED_STATUS_RUN, (uint8_t*)&status.run);
    ZING_get_status(status_values, 3, ZED_STATUS_CNT, (uint8_t*)&status.cnt);
    
    return status;
}

ZING_status ZING_zch_set_status(uint8_t** status_values)
{
    ZING_status status;
    
    status.usb = ZING_get_host_status_usb(status_values);
    status.vendor_id = ZING_get_host_status_vnd(status_values);
    status.product_id = ZING_get_host_status_prd(status_values);
    status.channel = ZING_get_host_status_bnd(status_values);
    status.ppid = ZING_get_host_status_ppid(status_values);
    status.device_id = ZING_get_host_status_devid(status_values);
    status.format = ZING_get_host_status_fmt(status_values);
    status.index = ZING_get_host_status_idx(status_values);
    status.trt = ZING_get_host_status_trt(status_values);
    status.ack = ZING_get_host_status_ack(status_values);
    status.ppc = ZING_get_host_status_ppc(status_values);
    status.txid = ZING_get_host_status_txid(status_values);
    status.rxid = ZING_get_host_status_rxid(status_values);
    status.run = ZING_get_host_status_run(status_values);
    status.cnt = ZING_get_host_status_cnt(status_values);
    
    return status;
}

ZING_status ZING_zcd_set_status(uint8_t** status_values)
{
    ZING_status status;
    
    status.usb = ZING_get_device_status_usb(status_values);
    status.ppid = ZING_get_device_status_ppid(status_values);
    status.device_id = ZING_get_device_status_devid(status_values);
    status.format = ZING_get_device_status_fmt(status_values);
    status.index = ZING_get_device_status_idx(status_values);
    status.fps = ZING_get_device_status_fps(status_values);
    status.trt = ZING_get_device_status_trt(status_values);
    status.ack = ZING_get_device_status_ack(status_values);
    status.ppc = ZING_get_device_status_ppc(status_values);
    status.run = ZING_get_device_status_run(status_values);
    status.itf = ZING_get_device_status_itf(status_values);
    status.txid = ZING_get_device_status_txid(status_values);
    status.rxid = ZING_get_device_status_rxid(status_values);
    status.destid_err_cnt = ZING_get_device_status_dst_id_err_cnt(status_values).cnt;
    status.phy_rx_frame_cnt = ZING_get_device_status_phy_rx_frame_cnt(status_values).cnt;
    status.mfir = (ZING_get_device_status_mfir(status_values).dst_id_err_diff << 16) | ZING_get_device_status_mfir(status_values).phy_rx_frame_diff;
    status.cnt = ZING_get_device_status_cnt(status_values);
    
    return status;
}