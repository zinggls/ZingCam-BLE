#ifndef AIAS_HBLE_H
#define AIAS_HBLE_H

#include <stdio.h>
#include <stdlib.h>
    
#include "project.h"
    
#include "../common/headers/imu.h"

typedef struct AIAS_IMU_FRAME_t
{
    IMU_VALUE r_z;
    IMU_VALUE p_y;
    IMU_VALUE y_x;
    IMU_VALUE ts_w;
    uint16_t timestamp;
} AIAS_IMU_FRAME;
    
typedef struct AIAS_ZING_FRAME_t
{
    uint8_t usb;
    uint32_t vnd;
    uint32_t prd;
    uint8_t bnd;
    uint32_t ppid;
    uint32_t device_id;
    uint8_t fmt;
    uint8_t idx;
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
} AIAS_ZING_FRAME;
    
typedef struct AIAS_BLE_FRAME_t
{
    int type;
    AIAS_IMU_FRAME imu_frame;
    AIAS_ZING_FRAME zing_frame;
} AIAS_BLE_FRAME;

void HBLE_init(void);
AIAS_ZING_FRAME HBLE_set_zing_frame(char* zing_status, char** status_values);
AIAS_IMU_FRAME HBLE_set_imu_frame(char* imu, char** imu_values, uint8_t type);
        
#endif