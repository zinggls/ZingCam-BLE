#include <stdio.h>

#include "project.h"

#define NUM_TOTAL_IMU_DATAS 7
#define MAX_IMU_DATAS_SIZE 40

void IMU_init(void);
uint8_t IMU_get(uint16_t* datas);