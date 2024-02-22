#ifndef BLE_H
#define BLE_H

#include <stdio.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

#define MAX_BLE_FRAME_SIZE 250
#define MAX_STATE_LENGTH 13
#define MAX_DATA_LENGTH 12

typedef struct
{
    uint8_t reserved0;
    uint8_t reserved1;
    uint8_t reserved2;
    uint8_t reserved3;
    uint8_t reserved4;
    uint8_t reserved5;
    uint8_t reserved6;
    uint8_t reserved7_6;
    uint8_t set_channel:1;
    uint8_t reset:1;
} ZING_parameters;
    
typedef struct
{
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    uint8_t status_values[(NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS) * MAX_DATA_LENGTH];
    //uint32_t status_values2[NUM_HOST_STATUS > NUM_DEVICE_STATUS ? NUM_HOST_STATUS : NUM_DEVICE_STATUS];
    ZING_parameters zing_params;
} ZCBLE_frame;

void ZCBLE_init(void);

#endif