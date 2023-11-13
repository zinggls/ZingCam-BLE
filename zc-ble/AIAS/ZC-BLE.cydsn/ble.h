#ifndef BLE_H
#define BLE_H

#include <stdio.h>

#include "project.h"
#include "imu.h"
#include "zing.h"

#define MAX_BLE_FRAME_SIZE 250
#define MAX_STATE_LENGTH 13

typedef struct
{
    uint16_t imu_values[NUM_TOTAL_IMU_VALUES];
    uint8_t status_values[NUM_HOST_STATUS * MAX_VALUE_LENGTH];
} ZCBLE_frame;

void ZCBLE_init(void);

#endif