#ifndef IMU_H
#define IMU_H

#include <stdio.h>

#include "project.h"

#define NUM_TOTAL_IMU_VALUES 12
#define MAX_IMU_VALUES_SIZE 48

void IMU_init(void);
uint8_t IMU_get(uint16_t* values);

#endif