#ifndef BLE_H
#define BLE_H

#include <stdio.h>

#include "project.h"
#include "imu.h"
#include "zing.h"
#include "main.h"
    
#define MAX_BLE_FRAME_SIZE 250
#define MAX_STATE_LENGTH 13
#define MAX_DATA_LENGTH 12

#define ZCBLE_NUM_ZING_STATUS (NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS)
#define ZCBLE_LENGTH_ZING_STATUS (ZCBLE_NUM_ZING_STATUS * MAX_DATA_LENGTH)
    
typedef struct
{
    uint8_t camera:4;
    uint8_t output:4;
} ZCBLE_scope;

typedef struct
{
    uint8_t mode:4;
    uint8_t info:4;
} ZCBLE_wireless_channel;

typedef struct
{
    uint8_t scope:4;
    uint8_t transmitter:4;
} ZCBLE_opmode;

typedef struct
{
    uint8_t status;
    uint8_t type:4;
    uint8_t calibrate:4;
} ZCBLE_transmitter_imu;

typedef struct
{
    uint8_t scope;
    uint8_t transmitter;
} ZCBLE_battery_level;

typedef struct
{
    uint8_t ir:4;
    uint8_t eo:4;
} ZCBLE_modules_status;

typedef struct
{
    uint8_t transmitter:4;
    uint8_t reserved:4;
} ZCBLE_modem;

typedef struct
{
    ZCBLE_scope scope;
    ZCBLE_wireless_channel w_c;
    ZCBLE_opmode opmode;
    ZCBLE_transmitter_imu tx_imu;
    ZCBLE_battery_level battey;
    ZCBLE_modules_status modules;
    ZCBLE_modem modem;
} ICD_parameters;

typedef struct
{
    uint8_t usb;
    uint16_t vendor_id;
    uint16_t product_id;
    uint8_t channel;
    uint16_t ppid;
    uint16_t device_id;
    uint8_t format;
    uint8_t index;
    uint32_t fps;
    uint8_t trt;
    uint8_t ack;
    uint8_t ppc;
    uint8_t run;
    uint8_t itf;
    uint32_t txid;
    uint32_t rxid;
    uint32_t destid_err_cnt;
    uint32_t phy_rx_frame_cnt;
    uint32_t mfir;
    uint32_t cnt;
} ZING_status;

typedef struct
{
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    ZING_status status;
    ICD_parameters icd_params;
} ZCBLE_frame;

#if HBLE
uint8_t** ZCBLE_get_zing_device_status_values(void);
#endif
#if DBLE
uint8_t** ZCBLE_get_zing_host_status_values(void);
#endif
void ZCBLE_init(void);

ZING_status ZING_zed_set_status(uint8_t** status_values);
ZING_status ZING_zch_set_status(uint8_t** status_values);
ZING_status ZING_zcd_set_status(uint8_t** status_values);

#endif