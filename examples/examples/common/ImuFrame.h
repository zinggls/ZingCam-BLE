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

void ImuFrame_init(ImuFrame *imu)
{
    memset(imu->data,0,IMU_FRAME_SIZE);
    imu->index = -1;
    imu->isEmpty = true;
    imu->isFull = false;
}

uint16_t ImuFrame_checksum(ImuFrame *imu)
{
    uint16_t checksum = 0;
    for (uint8_t i = 0; i < (IMU_FRAME_SIZE-2); i++) checksum += (imu->data[i]);
    return checksum;
}

int ImuFrame_integrity(ImuFrame *imu)
{
    uint16_t checksum = ImuFrame_checksum(imu);
    uint8_t high = (checksum&0xff00)>>8;
    uint8_t low = checksum&0xff;

    if(imu->data[IMU_FRAME_SIZE-2]!=high) return 0;
    if(imu->data[IMU_FRAME_SIZE-1]!=low) return 0;
    return 1;
}

#endif /* IMU_FRAME_H */