#ifndef IMU_H
#define IMU_H

#include <stdio.h>

#include "project.h"

#define NUM_TOTAL_IMU_EULER_VALUES 4 + 1
#define NUM_TOTAL_IMU_QUATERNION_VALUES 5 + 1
#define NUM_IMU_EULER_DATA_BYTES 10 + 2
#define NUM_IMU_QUATERNION_DATA_BYTES 12 + 2
#define NUM_IMU_SOF_UPPER 0
#define NUM_IMU_SOF_LOWER 1
    
#define IMU_EULER 0
#define IMU_QUATERNION 1
    
typedef enum
{
    IMU_SOF_0,
    IMU_SOF_1,
    IMU_SOF_2,
    IMU_SOF_3,
    IMU_SOTS,
    IMU_CHECKSUM,
    NUM_IMU_DATAS
} IMU_DATAS;
    
void IMU_init(void);
uint8_t IMU_get(uint16_t* values);
uint8_t IMU_get_type(void);
uint8_t IMU_get_calibrate(void);

void IMU_set_output_format(uint8_t sof);
void IMU_calibration_gyro(void);
void IMU_calibration_accelero_simple(void);
void IMU_calibration_magneto_free(void);

#endif