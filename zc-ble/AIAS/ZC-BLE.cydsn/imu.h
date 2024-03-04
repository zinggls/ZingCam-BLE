#ifndef IMU_H
#define IMU_H

#include <stdio.h>

#include "project.h"

#define NUM_TOTAL_IMU_VALUES 4
#define MAX_IMU_VALUES_SIZE 48

#define IMU_EULER 0
#define IMU_QUATERNION 1
    
typedef enum
{
    IMU_SOF_0,
    IMU_SOF_1,
    IMU_SOF_2,
    IMU_SOF_3,
    IMU_SOTS,
    NUM_IMU_DATAS
} IMU_DATAS;
    
void IMU_init(void);
uint8_t IMU_get(uint16_t* values);

void IMU_set_output_format(uint8_t sof);
void IMU_set_output_gyro(uint8_t sog);
void IMU_set_output_accelero(uint8_t soa);
void IMU_set_output_mageneto(uint8_t som);
void IMU_set_output_temperature(uint8_t sot);
void IMU_set_output_timestamp(uint8_t sots);
void IMU_calibration_gyro(void);
void IMU_calibration_accelero_free(void);
void IMU_calibration_accelero_simple(void);
void IMU_calibration_magneto_free(void);
void IMU_calibration_magneto_xy(void);
void IMU_calibration_magneto_pxy(void);
void IMU_calibration_magneto_z(void);
void IMU_calibration_magneto_pz(void);
void IMU_set_motion_offset(void);
void IMU_set_motion_offset_x(void);
void IMU_set_motion_offset_y(void);
void IMU_set_motion_offset_z(void);
void IMU_set_motion_offset_xy(void);
void IMU_clear_motion_offset(void);

#endif