#ifndef IMU_FRAME_H
#define IMU_FRAME_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define IMU_FRAME_SIZE 12

typedef struct {
    char data[IMU_FRAME_SIZE];
    int index;
    bool isEmpty;
    bool isFull;
}ImuFrame;

void ImuFrame_init(ImuFrame *imu);
uint16_t ImuFrame_checksum(ImuFrame *imu);
int ImuFrame_integrity(ImuFrame *imu);

#endif /* IMU_FRAME_H */