#include "icd.h"
#include "imu.h"
#include "zing.h"

#include <stdlib.h>

static uint8_t icd_imu_type = IMU_EULER;

static uint8_t AIAS_ICD_MAP[] =
{
#if HBLE
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00,
    
    // ZING HOST STATUS
    0x02,                   // USB
    0x12, 0x34,             // VND
    0x00, 0x00,             // PRD
    0x00,                   // BND
    0x00, 0x00,             // PPID
    0x00, 0x00,             // DEVID
    0x00,                   // FMT
    0x00,                   // IDX
    0x00,                   // TRT
    0x00,                   // ACK
    0x00,                   // PPC
    0x00,                   // RUN
    0x00, 0x00, 0x00, 0x00, // TXID
    0x00, 0x00, 0x00, 0x00, // RXID
    0x00, 0x00,             // CNT
    // ZING DEVICE STATUS
    0x00,                   // USB
    0x00, 0x00,             // PPID
    0x00, 0x00,             // DEVID
    0x00,                   // FMT
    0x00,                   // IDX
    0x00, 0x00, 0x00, 0x00, // FPS
    0x00,                   // TRT
    0x00,                   // ACK
    0x00,                   // PPC
    0x00,                   // RUN
    0x00,                   // ITF
    0x00, 0x00, 0x00, 0x00, // TXID
    0x00, 0x00, 0x00, 0x00, // RXID
    0x00, 0x00, 0x00, 0x00, // DST_ID_ERR_CNT
    0x00, 0x00,             // DST_ID_DIFF_CNT
    0x00, 0x00, 0x00, 0x00, // RX_FRAME_CNT
    0x00, 0x00,             // RX_FRAME_DIFF_CNT
    0x00, 0x00, 0x00, 0x00, // MFIR
    0x00, 0x00, // CNT
#endif    
#if DBLE
    // AIAS ICD
    0x00, 0x01, 0x02, 0x01, 0x01, 0x00, 0x01, 0x07, 0x08, 0x09,
    0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10, 0x11, 0x12, 0x13,
    0x16, 0x17, 0x18, 0x19, 0x1A, 0x1B, 0x1C, 0x1D, 0x1E, 0x1F, 0x14, 0x15,
    0x20, 0x21, 0x22, 0x23, 0x24, 0x25, 0x26, 0x27, 0x28, 0x29, 0x2A, 0x2B,
    // ZING HOST STATUS
    0x02,                   // USB
    0x12, 0x34,             // VND
    0x00, 0x00,             // PRD
    0x00,                   // BND
    0x00, 0x00,             // PPID
    0x00, 0x00,             // DEVID
    0x00,                   // FMT
    0x00,                   // IDX
    0x00,                   // TRT
    0x00,                   // ACK
    0x00,                   // PPC
    0x00,                   // RUN
    0x00, 0x00, 0x00, 0x00, // TXID
    0x00, 0x00, 0x00, 0x00, // RXID
    0x00, 0x00,             // CNT
    // ZING DEVICE STATUS
    0x00,                   // USB
    0x00, 0x00,             // PPID
    0x00, 0x00,             // DEVID
    0x00, 0x00, 0x00, 0x00, // FPS
    0x00,                   // FMT
    0x00,                   // IDX
    0x00,                   // TRT
    0x00,                   // ACK
    0x00,                   // PPC
    0x00,                   // RUN
    0x00,                   // ITF
    0x00, 0x00, 0x00, 0x00, // TXID
    0x00, 0x00, 0x00, 0x00, // RXID
    0x00, 0x00, 0x00, 0x00, // DST_ID_ERR_CNT
    0x00, 0x00,             // DST_ID_DIFF_CNT
    0x00, 0x00, 0x00, 0x00, // RX_FRAME_CNT
    0x00, 0x00,             // RX_FRAME_DIFF_CNT
    0x00, 0x00, 0x00, 0x00, // MFIR
    0x00, 0x00, // CNT
#endif
};

uint8_t* AIAS_ICD_get_map(void)
{
    return AIAS_ICD_MAP;
}

uint8_t AIAS_ICD_get(uint8_t addr)
{
    return AIAS_ICD_MAP[addr];
}

void AIAS_ICD_get_block(uint8_t addr, uint8_t* buffer, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        buffer[i] = AIAS_ICD_get(addr + i);
    }
}

void AIAS_ICD_set(uint8_t addr, uint8_t value)
{
    AIAS_ICD_MAP[addr] = value;
}

void AIAS_ICD_set_block(uint8_t addr, uint8_t* buffer, uint8_t len)
{
    for (uint8_t i = 0; i < len; i++)
    {
        AIAS_ICD_MAP[addr + i] = buffer[i];
    }
}

void AIAS_ICD_read(void)
{
    AI2C_set_read_buffer(AIAS_ICD_MAP, NUM_READ_AIAS_ICD);
    AI2C_read();
}

uint8_t AIAS_ICD_write(void)
{
    uint8_t ret;
    
    ret = AI2C_write();
    
    if (ret == 1)
    {
        AI2C_set_write_buffer(AIAS_ICD_MAP, NUM_WRITE_AIAS_ICD);
        
#if DBLE   
        switch (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_OUTPUT_TYPE))
        {
            case 0x00:
                IMU_set_output_format(IMU_EULER);
                break;
            case 0x01:
                IMU_set_output_format(IMU_QUATERNION);
                break;
        }

        switch (AIAS_ICD_get(WIRELESS_VIDEO_RECEIVER_IMU_CALIBRATE))
        {
            case 0x01:
                IMU_calibration_gyro();
                break;
            case 0x02:
                IMU_calibration_accelero_simple();
                break;
            case 0x03:
                IMU_calibration_magneto_free();
                break;
            case 0x04:
                UART_IMU_UartPutChar('>');
                break;
        }
        
        switch (AIAS_ICD_get(WIRELESS_VIDEO_CHANNEL_MODE))
        {
            case 0x01:
                ZING_set_mode(ZING_MODE_AUTO);
                break;
            case 0x02:
                ZING_set_mode(ZING_MODE_MANUAL);
                break;
            default:
                ZING_set_mode(ZING_MODE_MANUAL);
                break;
        }
        
        switch (AIAS_ICD_get(WIRELESS_VIDEO_CHANNEL_INFORMATION))
        {
            case 0x01:
                ZING_set_channel_low();
                break;
            case 0x02:
                ZING_set_channel_high();
                break;
            default:
                ZING_set_channel_low();
                break;
        }
#endif
    }
    return ret;

}

void AIAS_ICD_set_scope(ZCBLE_scope scope)
{
#if HBLE    
    switch (scope.camera)
    {
        case 1:
            // Turn EO
            break;
        case 2:
            // Turn IR
            break;
    }
    
    switch (scope.output)
    {
        case 0:
            // Output
            break;
        case 1:
            // No output
            break;
    }
#endif
    AIAS_ICD_set(SCOPE_CAMERA, scope.camera);
    AIAS_ICD_set(SCOPE_OUTPUT, scope.output);
}

#if HBLE
void AIAS_ICD_set_wireless_channel(ZCBLE_wireless_channel w_c)
{    
    ZING_set_mode(w_c.mode);
    
    switch (w_c.info)
    {
        case ZING_INFO_CH1:
            // Set channel 1
            ZING_set_channel_low();
            break;
        case ZING_INFO_CH2:
            // Set channel 2
            ZING_set_channel_high();
            break;
    }
}
#endif
#if DBLE
void AIAS_ICD_set_wireless_channel(ZCBLE_wireless_channel w_c)
{
    if (w_c.mode == 0)
    {
        return;
    }
    
    switch (w_c.info)
    {
        case ZING_INFO_CH1:
            ZING_set_channel_low();
            break;
        case ZING_INFO_CH2:
            ZING_set_channel_high();
            break;
    }
    
    AIAS_ICD_set(WIRELESS_VIDEO_CHANNEL_MODE, w_c.mode);
    AIAS_ICD_set(WIRELESS_VIDEO_CHANNEL_INFORMATION, w_c.info);
}
#endif

void AIAS_ICD_set_opmode(ZCBLE_opmode opmode)
{
#if HBLE
    switch (opmode.scope)
    {
        case 1:
            // Operate mode
            break;
        case 2:
            // Standby mode
            break;
        case 4:
            // Power save mode
            break;
    }
    
    switch (opmode.transmitter)
    {
        case 1:
            // Operate mode
            PW_EN_Write(1);
            break;
        case 2:
            // Standby mode
            // ZING is not used
            break;
        case 4:
            // Power save mode
            PW_EN_Write(0);
            break;
    }
#endif
#if DBLE
    AIAS_ICD_set(SCOPE_OPERATION_MODE, opmode.scope);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_OPERATION_MODE_STATUS, opmode.transmitter);
#endif
}

void AIAS_ICD_set_transitter_imu_status(uint8_t imu_status)
{
    uint8_t status;
    
    if (imu_status != 0x00)
    {
        status = imu_status | 0xE0;    
    }
    else
    {
        status = 0x00;
    }
#if DBLE
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_STATUS, status);
#endif
}

void AIAS_ICD_set_transitter_imu(ZCBLE_transmitter_imu imu)
{
#if HBLE
    switch (imu.type)
    {
        case 0:
            icd_imu_type = IMU_EULER;
            IMU_set_output_format(IMU_EULER);
            break;
        case 1:
            icd_imu_type = IMU_QUATERNION;
            IMU_set_output_format(IMU_QUATERNION);
            break;
        default:
            break;
    }
    
    switch (imu.calibrate)
    {
        case 0x01:
            IMU_calibration_gyro();
            break;
        case 0x02:
            IMU_calibration_accelero_simple();
            break;
        case 0x03:
            IMU_calibration_magneto_free();
            break;
        case 0x04:
            UART_IMU_UartPutChar('>'); // calibrate done
            break;
    }
#endif
#if DBLE
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_OUTPUT_TYPE, imu.type);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_CALIBRATE, imu.calibrate);
#endif
}

void AIAS_ICD_set_zcble_frame(ZCBLE_frame* zcble_frame)
{
#if HBLE
    zcble_frame->icd_params.scope.camera = AIAS_ICD_MAP[SCOPE_CAMERA];
    zcble_frame->icd_params.scope.output = AIAS_ICD_MAP[SCOPE_OUTPUT];
    zcble_frame->icd_params.w_c.mode = ZING_get_mode();
    zcble_frame->icd_params.w_c.info = ZING_get_info();
    zcble_frame->icd_params.opmode.scope = AIAS_ICD_MAP[SCOPE_OPERATION_MODE];
    zcble_frame->icd_params.opmode.transmitter = 0x1;
    zcble_frame->icd_params.tx_imu.type = IMU_get_type();
    zcble_frame->icd_params.tx_imu.calibrate = IMU_get_calibrate();
    zcble_frame->icd_params.battey.scope = AIAS_ICD_MAP[SCOPE_BATTERY_STATUS];
    zcble_frame->icd_params.modules.ir = AIAS_ICD_MAP[SCOPE_IR_MODULE_STATUS];
    zcble_frame->icd_params.modules.eo = AIAS_ICD_MAP[SCOPE_EO_MODULE_STATUS];
#endif
#if DBLE
    zcble_frame->icd_params.scope.camera = AIAS_ICD_MAP[SCOPE_CAMERA];
    zcble_frame->icd_params.scope.output = AIAS_ICD_MAP[SCOPE_OUTPUT];
    zcble_frame->icd_params.w_c.mode = AIAS_ICD_MAP[WIRELESS_VIDEO_CHANNEL_MODE];
    zcble_frame->icd_params.w_c.info = AIAS_ICD_MAP[WIRELESS_VIDEO_CHANNEL_INFORMATION];
    zcble_frame->icd_params.opmode.scope = AIAS_ICD_MAP[SCOPE_OPERATION_MODE];
    zcble_frame->icd_params.tx_imu.type = AIAS_ICD_MAP[WIRELESS_VIDEO_TRANSMITTER_IMU_OUTPUT_TYPE];
    zcble_frame->icd_params.tx_imu.calibrate = AIAS_ICD_MAP[WIRELESS_VIDEO_TRANSMITTER_IMU_CALIBRATE];
#endif
}

#if DBLE
void AIAS_ICD_set_battery_level(ZCBLE_battery_level battery)
{
    AIAS_ICD_set(SCOPE_BATTERY_STATUS, battery.scope);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_BATTERY_STATUS, battery.transmitter);
}

void AIAS_ICD_set_modules_status(ZCBLE_modules_status modules)
{
    AIAS_ICD_set(SCOPE_IR_MODULE_STATUS, modules.ir);
    AIAS_ICD_set(SCOPE_EO_MODULE_STATUS, modules.eo);
}

void AIAS_ICD_set_modem_status(ZCBLE_modem modem)
{
    uint8_t status;
    
    if (modem.transmitter != 0x00)
    {
        status = modem.transmitter | 0xE0;    
    }
    else
    {
        status = 0x00;
    }
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_MODEM_STATUS, status);
}

#endif

// TEST

#if DBLE
void AIAS_ICD_set_receiver_imu_data(uint8_t sof, uint16_t* imu_values)
{
    if (sof == IMU_EULER)
    {
        AIAS_ICD_set_receiver_imu_data1(IMU_EULER, 0x00);
        AIAS_ICD_set_receiver_imu_data2(imu_values[IMU_SOF_0]);
        AIAS_ICD_set_receiver_imu_data3(imu_values[IMU_SOF_1]);
        AIAS_ICD_set_receiver_imu_data4(imu_values[IMU_SOF_2]);
        AIAS_ICD_set_receiver_imu_data5(imu_values[IMU_SOF_3]);
        AIAS_ICD_set_receiver_imu_checksum(imu_values[IMU_SOTS]);
    }
    else
    {
        AIAS_ICD_set_receiver_imu_data1(IMU_QUATERNION, imu_values[IMU_SOF_0]);
        AIAS_ICD_set_receiver_imu_data2(imu_values[IMU_SOF_1]);
        AIAS_ICD_set_receiver_imu_data3(imu_values[IMU_SOF_2]);
        AIAS_ICD_set_receiver_imu_data4(imu_values[IMU_SOF_3]);
        AIAS_ICD_set_receiver_imu_data5(imu_values[IMU_SOTS]);
        AIAS_ICD_set_receiver_imu_checksum(imu_values[IMU_CHECKSUM]);
    }
}

void AIAS_ICD_set_receiver_imu_data1(uint8_t sof, uint16_t data1)
{
    uint8_t data_h;
    uint8_t data_l;
    
    if (sof == IMU_EULER)
    {
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA1_H, 0x00);
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA1_L, 0x00);
    }
    else if (sof == IMU_QUATERNION)
    {
        data_h = (data1 & 0xFF00) >> 8;
        data_l = (data1 & 0x00FF);
        
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA1_H, data_h);
        AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA1_L, data_l);
    }
}

void AIAS_ICD_set_receiver_imu_data2(uint16_t data2)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data2 & 0xFF00) >> 8;
    data_l = (data2 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA2_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA2_L, data_l);
}

void AIAS_ICD_set_receiver_imu_data3(uint16_t data3)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data3 & 0xFF00) >> 8;
    data_l = (data3 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA3_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA3_L, data_l);
}

void AIAS_ICD_set_receiver_imu_data4(uint16_t data4)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data4 & 0xFF00) >> 8;
    data_l = (data4 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA4_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA4_L, data_l);
}

void AIAS_ICD_set_receiver_imu_data5(uint16_t data5)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data5 & 0xFF00) >> 8;
    data_l = (data5 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA5_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_DATA5_L, data_l);
}

void AIAS_ICD_set_receiver_imu_checksum(uint16_t checksum)
{
    uint8_t checksum_h;
    uint8_t checksum_l;
    
    checksum_h = (checksum & 0xFF00) >> 8;
    checksum_l = (checksum & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_CHECKSUM_UPPER, checksum_h);
    AIAS_ICD_set(WIRELESS_VIDEO_RECEIVER_IMU_CHECKSUM_LOWER, checksum_l);
}

void AIAS_ICD_set_transmitter_imu_data(uint8_t sof, uint16_t* imu_values)
{
    if (sof == IMU_EULER)
    {
        AIAS_ICD_set_transmitter_imu_data1(IMU_EULER, 0x00);
        AIAS_ICD_set_transmitter_imu_data2(imu_values[IMU_SOF_0]);
        AIAS_ICD_set_transmitter_imu_data3(imu_values[IMU_SOF_1]);
        AIAS_ICD_set_transmitter_imu_data4(imu_values[IMU_SOF_2]);
        AIAS_ICD_set_transmitter_imu_data5(imu_values[IMU_SOF_3]);
        AIAS_ICD_set_transmitter_imu_checksum(imu_values[IMU_SOTS]);
    }
    else
    {
        AIAS_ICD_set_transmitter_imu_data1(IMU_QUATERNION, imu_values[IMU_SOF_0]);
        AIAS_ICD_set_transmitter_imu_data2(imu_values[IMU_SOF_1]);
        AIAS_ICD_set_transmitter_imu_data3(imu_values[IMU_SOF_2]);
        AIAS_ICD_set_transmitter_imu_data4(imu_values[IMU_SOF_3]);
        AIAS_ICD_set_transmitter_imu_data5(imu_values[IMU_SOTS]);
        AIAS_ICD_set_transmitter_imu_checksum(imu_values[IMU_CHECKSUM]);
    }
}

void AIAS_ICD_set_transmitter_imu_data1(uint8_t sof, uint16_t data1)
{
    uint8_t data_h;
    uint8_t data_l;
    
    if (sof == IMU_EULER)
    {
        AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA1_H, 0x00);
        AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA1_L, 0x00);
    }
    else if (sof == IMU_QUATERNION)
    {
        data_h = (data1 & 0xFF00) >> 8;
        data_l = (data1 & 0x00FF);
        
        AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA1_H, data_h);
        AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA1_L, data_l);
    }
}

void AIAS_ICD_set_transmitter_imu_data2(uint16_t data2)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data2 & 0xFF00) >> 8;
    data_l = (data2 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA2_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA2_L, data_l);
}

void AIAS_ICD_set_transmitter_imu_data3(uint16_t data3)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data3 & 0xFF00) >> 8;
    data_l = (data3 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA3_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA3_L, data_l);
}

void AIAS_ICD_set_transmitter_imu_data4(uint16_t data4)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data4 & 0xFF00) >> 8;
    data_l = (data4 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA4_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA4_L, data_l);
}

void AIAS_ICD_set_transmitter_imu_data5(uint16_t data5)
{
    uint8_t data_h;
    uint8_t data_l;
    
    data_h = (data5 & 0xFF00) >> 8;
    data_l = (data5 & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA5_H, data_h);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_DATA5_L, data_l);
}

void AIAS_ICD_set_transmitter_imu_checksum(uint16_t checksum)
{
    uint8_t checksum_h;
    uint8_t checksum_l;
    
    checksum_h = (checksum & 0xFF00) >> 8;
    checksum_l = (checksum & 0x00FF);
    
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_CHECKSUM_UPPER, checksum_h);
    AIAS_ICD_set(WIRELESS_VIDEO_TRANSMITTER_IMU_CHECKSUM_LOWER, checksum_l);
}
#endif 

void AIAS_ICD_update_host_status(ZING_status status, uint8_t** ptr)
{
    AIAS_ICD_set_host_status_usb(status, ptr);
    AIAS_ICD_set_host_status_vnd(status, ptr);
    AIAS_ICD_set_host_status_prd(status, ptr);
    AIAS_ICD_set_host_status_bnd(status, ptr);
    AIAS_ICD_set_host_status_ppid(status, ptr);
    AIAS_ICD_set_host_status_devid(status, ptr);
    AIAS_ICD_set_host_status_fmt(status, ptr);
    AIAS_ICD_set_host_status_idx(status, ptr);
    AIAS_ICD_set_host_status_trt(status, ptr);
    AIAS_ICD_set_host_status_ack(status, ptr);
    AIAS_ICD_set_host_status_ppc(status, ptr);
    AIAS_ICD_set_host_status_txid(status, ptr);
    AIAS_ICD_set_host_status_rxid(status, ptr);
    AIAS_ICD_set_host_status_run(status, ptr);
    AIAS_ICD_set_host_status_cnt(status, ptr);
}

void AIAS_ICD_set_host_status_usb(ZING_status status, uint8_t** ptr)
{
    uint8_t usb;
    
    if (ptr == NULL)
    {
        usb = status.usb;
    }
    else
    {
        usb = ZING_get_host_status_usb(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_USB] = usb;
}

void AIAS_ICD_set_host_status_vnd(ZING_status status, uint8_t** ptr)
{
    uint16_t vnd;
    uint8_t VND_H;
    uint8_t VND_L;
    
    if (ptr == NULL)
    {
        vnd = status.vendor_id;
    }
    else
    {
        vnd = ZING_get_host_status_vnd(ptr);
    }
    VND_H = (vnd & 0xFF00) >> 8;
    VND_L = (vnd & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_VND_H] = VND_H;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_VND_L] = VND_L;
}

void AIAS_ICD_set_host_status_prd(ZING_status status, uint8_t** ptr)
{
    uint16_t prd;
    uint8_t PRD_H;
    uint8_t PRD_L;
    
    if (ptr == NULL)
    {
        prd = status.product_id;
    }
    else
    {
        prd = ZING_get_host_status_prd(ptr);
    }
    PRD_H = (prd& 0xFF00) >> 8;
    PRD_L = (prd & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_PRD_H] = PRD_H;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_PRD_L] = PRD_L;
}

void AIAS_ICD_set_host_status_bnd(ZING_status status, uint8_t** ptr)
{
    char bnd;
    
    if (ptr == NULL)
    {
        bnd = status.channel;
    }
    else
    {
        bnd = ZING_get_host_status_bnd(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_BND] = bnd;
}

void AIAS_ICD_set_host_status_ppid(ZING_status status, uint8_t** ptr)
{
    uint16_t ppid;
    uint8_t PPID_H;
    uint8_t PPID_L;
    
    if (ptr == NULL)
    {
        ppid = status.ppid;
    }
    else
    {
        ppid = ZING_get_host_status_ppid(ptr);
    }
    PPID_H = (ppid & 0xFF00) >> 8;
    PPID_L = (ppid & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_PPID_H] = PPID_H;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_PPID_L] = PPID_L;
}

void AIAS_ICD_set_host_status_devid(ZING_status status, uint8_t** ptr)
{
    uint16_t devid;
    uint8_t DEVID_H;
    uint8_t DEVID_L;
    
    if (ptr == NULL)
    {
        devid = status.device_id;
    }
    else
    {
        devid = ZING_get_host_status_devid(ptr);
    }
    DEVID_H = (devid & 0xFF00) >> 8;
    DEVID_L = (devid & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_DEVID_H] = DEVID_H;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_DEVID_L] = DEVID_L;
}

void AIAS_ICD_set_host_status_fmt(ZING_status status, uint8_t** ptr)
{
    uint8_t fmt;
    
    if (ptr == NULL)
    {
        fmt = status.format;
    }
    else
    {
        fmt = ZING_get_host_status_fmt(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_FMT] = fmt;
}

void AIAS_ICD_set_host_status_idx(ZING_status status, uint8_t** ptr)
{
    uint8_t idx;
    
    if (ptr == NULL)
    {
        idx = status.index;
    }
    else
    {
        idx = ZING_get_host_status_idx(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_IDX] = idx;
}

void AIAS_ICD_set_host_status_trt(ZING_status status, uint8_t** ptr)
{
    char trt;
    
    if (ptr == NULL)
    {
        trt = status.trt;
    }
    else
    {
        trt = ZING_get_host_status_trt(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_TRT] = trt;
}

void AIAS_ICD_set_host_status_ack(ZING_status status, uint8_t** ptr)
{
    char ack;
    
    if (ptr == NULL)
    {
        ack = status.ack;
    }
    else
    {
        ack = ZING_get_host_status_ack(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_ACK] = ack;
}

void AIAS_ICD_set_host_status_ppc(ZING_status status, uint8_t** ptr)
{
    char ppc;
    
    if (ptr == NULL)
    {
        ppc = status.ppc;
    }
    else
    {
        ppc = ZING_get_host_status_ppc(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_PPC] = ppc;
}

void AIAS_ICD_set_host_status_txid(ZING_status status, uint8_t** ptr)
{
    uint32_t txid;
    uint8_t TXID_HH;
    uint8_t TXID_HL;
    uint8_t TXID_LH;
    uint8_t TXID_LL;
    
    if (ptr == NULL)
    {
        txid = status.txid;
    }
    else
    {
        txid = ZING_get_host_status_txid(ptr);
    }
    TXID_HH = (txid & 0xFF000000) >> 24;
    TXID_HL = (txid & 0x00FF0000) >> 16;
    TXID_LH = (txid & 0x0000FF00) >> 8;
    TXID_LL = (txid & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_TXID_HH] = TXID_HH;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_TXID_HL] = TXID_HL;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_TXID_LH] = TXID_LH;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_TXID_LL] = TXID_LL;
}

void AIAS_ICD_set_host_status_rxid(ZING_status status, uint8_t** ptr)
{
    uint32_t rxid;
    uint8_t RXID_HH;
    uint8_t RXID_HL;
    uint8_t RXID_LH;
    uint8_t RXID_LL;
    
    if (ptr == NULL)
    {
        rxid = status.rxid;
    }
    else
    {
        rxid = ZING_get_host_status_rxid(ptr);
    }
    RXID_HH = (rxid & 0xFF000000) >> 24;
    RXID_HL = (rxid & 0x00FF0000) >> 16;
    RXID_LH = (rxid & 0x0000FF00) >> 8;
    RXID_LL = (rxid & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_RXID_HH] = RXID_HH;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_RXID_HL] = RXID_HL;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_RXID_LH] = RXID_LH;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_RXID_LL] = RXID_LL;
}

void AIAS_ICD_set_host_status_run(ZING_status status, uint8_t** ptr)
{
    char run;
    
    if (ptr == NULL)
    {
        run = status.run;
    }
    else
    {
        run = ZING_get_host_status_run(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_RUN] = run;
}

void AIAS_ICD_set_host_status_cnt(ZING_status status, uint8_t** ptr)
{
    uint16_t cnt;
    uint8_t CNT_H;
    uint8_t CNT_L;
    
    if (ptr == NULL)
    {
        cnt = status.cnt;
    }
    else
    {
        cnt = ZING_get_host_status_cnt(ptr);
    }
    CNT_H = (cnt & 0xFF00) >> 8;
    CNT_L = (cnt & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_CNT_H] = CNT_H;
    AIAS_ICD_MAP[AIAS_ZING_HOST_STATUS_CNT_L] = CNT_L;
}

void AIAS_ICD_update_device_status(ZING_status status, uint8_t** ptr)
{
    AIAS_ICD_set_device_status_usb(status, ptr);
    AIAS_ICD_set_device_status_ppid(status, ptr);
    AIAS_ICD_set_device_status_devid(status, ptr);
    AIAS_ICD_set_device_status_fps(status, ptr);
    AIAS_ICD_set_device_status_fmt(status, ptr);
    AIAS_ICD_set_device_status_idx(status, ptr);
    AIAS_ICD_set_device_status_trt(status, ptr);
    AIAS_ICD_set_device_status_ack(status, ptr);
    AIAS_ICD_set_device_status_ppc(status, ptr);
    AIAS_ICD_set_device_status_run(status, ptr);
    AIAS_ICD_set_device_status_itf(status, ptr);
    AIAS_ICD_set_device_status_txid(status, ptr);
    AIAS_ICD_set_device_status_rxid(status, ptr);
    AIAS_ICD_set_device_status_dst_id_err_cnt(status, ptr);
    AIAS_ICD_set_device_status_phy_rx_frame_cnt(status, ptr);
    AIAS_ICD_set_device_status_mfir(status, ptr);
    AIAS_ICD_set_device_status_cnt(status, ptr);
}

void AIAS_ICD_set_device_status_usb(ZING_status status, uint8_t** ptr)
{
    uint8_t usb;
    
    if (ptr == NULL)
    {
        usb = status.usb;
    }
    else
    {
        usb = ZING_get_device_status_usb(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_USB] = usb;
}

void AIAS_ICD_set_device_status_ppid(ZING_status status, uint8_t** ptr)
{
    uint16_t ppid;
    uint8_t PPID_H;
    uint8_t PPID_L;
    
    if (ptr == NULL)
    {
        ppid = status.ppid;
    }
    else
    {
        ppid = ZING_get_device_status_ppid(ptr);
    }
    PPID_H = (ppid & 0xFF00) >> 8;
    PPID_L = (ppid & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PPID_H] = PPID_H;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PPID_L] = PPID_L;
}

void AIAS_ICD_set_device_status_devid(ZING_status status, uint8_t** ptr)
{
    uint16_t devid;
    uint8_t DEVID_H;
    uint8_t DEVID_L;
    
    if (ptr == NULL)
    {
        devid = status.device_id;
    }
    else
    {
        devid = ZING_get_device_status_devid(ptr);
    }
    DEVID_H = (devid & 0xFF00) >> 8;
    DEVID_L = (devid & 0x00FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DEVID_H] = DEVID_H;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DEVID_L] = DEVID_L;
}

void AIAS_ICD_set_device_status_fps(ZING_status status, uint8_t** ptr)
{
    uint32_t fps;
    uint8_t FPS_HH;
    uint8_t FPS_HL;
    uint8_t FPS_LH;
    uint8_t FPS_LL;
    
    if (ptr == NULL)
    {
        fps = status.fps;
    }
    else
    {
        fps = ZING_get_device_status_fps(ptr);
    }
    FPS_HH = (fps & 0xFF000000) >> 24;
    FPS_HL = (fps & 0x00FF0000) >> 16;
    FPS_LH = (fps & 0x0000FF00) >> 8;
    FPS_LL = (fps & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_FPS_HH] = FPS_HH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_FPS_HL] = FPS_HL;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_FPS_LH] = FPS_LH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_FPS_LL] = FPS_LL;
}

void AIAS_ICD_set_device_status_fmt(ZING_status status, uint8_t** ptr)
{
    uint8_t fmt;
    
    if (ptr == NULL)
    {
        fmt = status.format;
    }
    else
    {
        fmt = ZING_get_device_status_fmt(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_FMT] = fmt;
}

void AIAS_ICD_set_device_status_idx(ZING_status status, uint8_t** ptr)
{
    uint8_t idx;
    
    if (ptr == NULL)
    {
        idx = status.index;
    }
    else
    {
        idx = ZING_get_device_status_idx(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_IDX] = idx;
}

void AIAS_ICD_set_device_status_trt(ZING_status status, uint8_t** ptr)
{
    char trt;
    
    if (ptr == NULL)
    {
        trt = status.trt;
    }
    else
    {
        trt = ZING_get_device_status_trt(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_TRT] = trt;
}

void AIAS_ICD_set_device_status_ack(ZING_status status, uint8_t** ptr)
{
    char ack;
    
    if (ptr == NULL)
    {
        ack = status.ack;
    }
    else
    {
        ack = ZING_get_device_status_ack(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_ACK] = ack;
}

void AIAS_ICD_set_device_status_ppc(ZING_status status, uint8_t** ptr)
{
    char ppc;
    
    if (ptr == NULL)
    {
        ppc = status.ppc;
    }
    else
    {
        ppc = ZING_get_device_status_ppc(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PPC] = ppc;
}

void AIAS_ICD_set_device_status_run(ZING_status status, uint8_t** ptr)
{
    char run;
    
    if (ptr == NULL)
    {
        run = status.run;
    }
    else
    {
        run = ZING_get_device_status_run(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_RUN] = run;
}

void AIAS_ICD_set_device_status_itf(ZING_status status, uint8_t** ptr)
{
    char itf;
    
    if (ptr == NULL)
    {
        itf = status.itf;
    }
    else
    {
        itf = ZING_get_device_status_itf(ptr);
    }
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_ITF] = itf;
}

void AIAS_ICD_set_device_status_txid(ZING_status status, uint8_t** ptr)
{
    uint32_t txid;
    uint8_t TXID_HH;
    uint8_t TXID_HL;
    uint8_t TXID_LH;
    uint8_t TXID_LL;
    
    if (ptr == NULL)
    {
        txid = status.txid;
    }
    else
    {
        txid = ZING_get_device_status_txid(ptr);
    }
    TXID_HH = (txid & 0xFF000000) >> 24;
    TXID_HL = (txid & 0x00FF0000) >> 16;
    TXID_LH = (txid & 0x0000FF00) >> 8;
    TXID_LL = (txid & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_TXID_HH] = TXID_HH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_TXID_HL] = TXID_HL;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_TXID_LH] = TXID_LH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_TXID_LL] = TXID_LL;
}

void AIAS_ICD_set_device_status_rxid(ZING_status status, uint8_t** ptr)
{
    uint32_t rxid;
    uint8_t RXID_HH;
    uint8_t RXID_HL;
    uint8_t RXID_LH;
    uint8_t RXID_LL;
    
    if (ptr == NULL)
    {
        rxid = status.rxid;
    }
    else
    {
        rxid = ZING_get_device_status_rxid(ptr);
    }
    RXID_HH = (rxid & 0xFF000000) >> 24;
    RXID_HL = (rxid & 0x00FF0000) >> 16;
    RXID_LH = (rxid & 0x0000FF00) >> 8;
    RXID_LL = (rxid & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_RXID_HH] = RXID_HH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_RXID_HL] = RXID_HL;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_RXID_LH] = RXID_LH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_RXID_LL] = RXID_LL;
}

void AIAS_ICD_set_device_status_dst_id_err_cnt(ZING_status status, uint8_t** ptr)
{
    uint32_t dst_id_err_cnt;
    uint8_t DST_ID_ERR_CNT_HH;
    uint8_t DST_ID_ERR_CNT_HL;
    uint8_t DST_ID_ERR_CNT_LH;
    uint8_t DST_ID_ERR_CNT_LL;
    
    if (ptr == NULL)
    {
        dst_id_err_cnt = status.destid_err_cnt;
    }
    else
    {
        dst_id_err_cnt = ZING_get_device_status_dst_id_err_cnt(ptr).cnt;
    }
    DST_ID_ERR_CNT_HH = (dst_id_err_cnt & 0xFF000000) >> 24;
    DST_ID_ERR_CNT_HL = (dst_id_err_cnt & 0x00FF0000) >> 16;
    DST_ID_ERR_CNT_LH = (dst_id_err_cnt & 0x0000FF00) >> 8;
    DST_ID_ERR_CNT_LL = (dst_id_err_cnt & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DST_ID_ERR_CNT_HH] = DST_ID_ERR_CNT_HH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DST_ID_ERR_CNT_HL] = DST_ID_ERR_CNT_HL;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DST_ID_ERR_CNT_LH] = DST_ID_ERR_CNT_LH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_DST_ID_ERR_CNT_LL] = DST_ID_ERR_CNT_LL;
}

void AIAS_ICD_set_device_status_phy_rx_frame_cnt(ZING_status status, uint8_t** ptr)
{
    uint32_t phy_rx_frame_cnt;
    uint8_t PHY_RX_FRAME_CNT_HH;
    uint8_t PHY_RX_FRAME_CNT_HL;
    uint8_t PHY_RX_FRAME_CNT_LH;
    uint8_t PHY_RX_FRAME_CNT_LL;
    
    if (ptr == NULL)
    {
        phy_rx_frame_cnt = status.phy_rx_frame_cnt;
    }
    else
    {
        phy_rx_frame_cnt = ZING_get_device_status_phy_rx_frame_cnt(ptr).cnt;
    }
    PHY_RX_FRAME_CNT_HH = (phy_rx_frame_cnt & 0xFF000000) >> 24;
    PHY_RX_FRAME_CNT_HL = (phy_rx_frame_cnt & 0x00FF0000) >> 16;
    PHY_RX_FRAME_CNT_LH = (phy_rx_frame_cnt & 0x0000FF00) >> 8;
    PHY_RX_FRAME_CNT_LL = (phy_rx_frame_cnt & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PHY_RX_FRAME_CNT_HH] = PHY_RX_FRAME_CNT_HH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PHY_RX_FRAME_CNT_HL] = PHY_RX_FRAME_CNT_HL;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PHY_RX_FRAME_CNT_LH] = PHY_RX_FRAME_CNT_LH;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_PHY_RX_FRAME_CNT_LL] = PHY_RX_FRAME_CNT_LL;
}

void AIAS_ICD_set_device_status_mfir(ZING_status status, uint8_t** ptr)
{
    uint8_t MFIR_DST_ID_ERR_DIFF_H;
    uint8_t MFIR_DST_ID_ERR_DIFF_L;
    uint8_t MFIR_PHY_RX_FRAME_DIFF_H;
    uint8_t MFIR_PHY_RX_FRAME_DIFF_L;
    
    if (ptr == NULL)
    {
        uint32_t mfir;
        
        mfir = status.usb;
        MFIR_DST_ID_ERR_DIFF_H = (mfir & 0xFF000000) >> 24;
        MFIR_DST_ID_ERR_DIFF_L = (mfir & 0x00FF0000) >> 16;
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_HH] = MFIR_DST_ID_ERR_DIFF_H;
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_HL] = MFIR_DST_ID_ERR_DIFF_L;
        MFIR_PHY_RX_FRAME_DIFF_H = (mfir & 0x0000FF00) >> 8;
        MFIR_PHY_RX_FRAME_DIFF_L = (mfir & 0x000000FF);
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_LH] = MFIR_PHY_RX_FRAME_DIFF_H;
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_LL] = MFIR_PHY_RX_FRAME_DIFF_L;
    }
    else
    {
        ZING_mfir mfir;
        
        mfir = ZING_get_device_status_mfir(ptr);
        MFIR_DST_ID_ERR_DIFF_H = (mfir.dst_id_err_diff & 0xFF00) >> 8;
        MFIR_DST_ID_ERR_DIFF_L = (mfir.dst_id_err_diff & 0x00FF);
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_HH] = MFIR_DST_ID_ERR_DIFF_H;
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_HL] = MFIR_DST_ID_ERR_DIFF_L;
        MFIR_PHY_RX_FRAME_DIFF_H = (mfir.phy_rx_frame_diff & 0xFF00) >> 8;
        MFIR_PHY_RX_FRAME_DIFF_L = (mfir.phy_rx_frame_diff & 0x00FF);
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_LH] = MFIR_PHY_RX_FRAME_DIFF_H;
        AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_MFIR_LL] = MFIR_PHY_RX_FRAME_DIFF_L;
    }
}

void AIAS_ICD_set_device_status_cnt(ZING_status status, uint8_t** ptr)
{
    uint16_t cnt;
    uint8_t CNT_H;
    uint8_t CNT_L;
    
    if (ptr == NULL)
    {
        cnt = status.cnt;
    }
    else
    {
        cnt = ZING_get_device_status_cnt(ptr);
    }
    CNT_H = (cnt & 0x0000FF00) >> 8;
    CNT_L = (cnt & 0x000000FF);
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_CNT_H] = CNT_H;
    AIAS_ICD_MAP[AIAS_ZING_DEVICE_STATUS_CNT_L] = CNT_L;
}
