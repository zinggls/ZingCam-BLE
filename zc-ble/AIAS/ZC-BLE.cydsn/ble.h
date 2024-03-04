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

typedef struct
{
    uint8_t battery_level;
    
    uint8_t scope_camera:4;
    uint8_t scope_format:4;
    
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
    
    
    uint8_t calibrate_imu:5;
    uint8_t imu_output:1;
    uint8_t imu_cali_status:1;
    uint8_t imu_error:1;
    
    uint8_t reserved7:3;
    uint8_t zing_error:1;
    uint8_t auto_channel;
    uint8_t set_channel:2;
    uint8_t reset:1;
} ZING_parameters;
    
typedef struct
{
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    uint8_t status_values[(NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS) * MAX_DATA_LENGTH];
    //uint32_t status_values2[NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS];
    ZING_parameters zing_params;
} ZCBLE_frame;

#if HBLE
uint8_t** ZCBLE_get_zing_device_status_values(void);
#endif
#if DBLE
uint8_t** ZCBLE_get_zing_host_status_values(void);
#endif
void ZCBLE_init(void);

#endif