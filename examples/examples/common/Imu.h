#ifndef IMU_H
#define IMU_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define IMU_FRAME_SIZE 12

typedef struct {
    char data[IMU_FRAME_SIZE];
    int index;
    bool isEmpty;
    bool isFull;
}Imu;

void Imu_init(Imu *im)
{
    memset(im->data,0,IMU_FRAME_SIZE);
    im->index = -1;
    im->isEmpty = true;
    im->isFull = false;
}

uint16_t Imu_checksum(Imu *im)
{
    uint16_t checksum = 0;
    for (uint8_t i = 0; i < (IMU_FRAME_SIZE-2); i++) checksum += (im->data[i]);
    return checksum;
}

int Imu_integrity(Imu *im)
{
    uint16_t checksum = Imu_checksum(im);
    uint8_t high = (checksum&0xff00)>>8;
    uint8_t low = checksum&0xff;

    if(im->data[IMU_FRAME_SIZE-2]!=high) return 0;
    if(im->data[IMU_FRAME_SIZE-1]!=low) return 0;
    return 1;
}

#endif /* IMU_H */