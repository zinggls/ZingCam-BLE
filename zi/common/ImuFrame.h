#ifndef IMU_FRAME_H
#define IMU_FRAME_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

#define IMU_FRAME_SIZE 12
#define IMU_QUATERNION_FRAME_SIZE (IMU_FRAME_SIZE+2)

typedef struct {
    char data[IMU_QUATERNION_FRAME_SIZE];
    int index;
    bool isEmpty;
    bool isFull;
}ImuFrame;

void ImuFrame_setSof(uint8_t val);
uint8_t ImuFrame_size();
void ImuFrame_init(ImuFrame *imu);
uint16_t ImuFrame_checksum(ImuFrame *imu);
int ImuFrame_integrity(ImuFrame *imu);
uint16_t ImuFrame_checksum2(ImuFrame *imu,uint8_t imuFrameSize);
int ImuFrame_integrity2(ImuFrame *imu,uint8_t imuFrameSize);

#endif /* IMU_FRAME_H */