#ifndef AIAS_IMU_H
#define AIAS_IMU_H

#include "project.h"

#define NUM_EULER_IMU 4
#define NUM_QUATERNION_IMU 5
#define MAX_IMU_RECV_LENGTH 100
#define MAX_IMU_VALUE_LENGTH 8
#define MAX_IMU_TIMEOUT_MS 1000

typedef enum
{
    IMU_R_Z,
    IMU_P_Y,
    IMU_Y_X,
    IMU_TS_W,
    IMU___TS,
    NUM_IMU,
} IMU_t;
    
typedef struct IMU_VALUE_t
{
    uint8_t neg;
    uint16_t upper;
    uint16_t lower;
} IMU_VALUE;

void IMU_init(void);
uint8_t IMU_recv(char* imu);
uint8_t IMU_parse(char* imu, char** imu_values, uint8_t type);
IMU_VALUE IMU_parse_value(char* imu_values);
    
#endif