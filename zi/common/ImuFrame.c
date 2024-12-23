#include <project.h>
#include "ImuFrame.h"

static uint8_t sof = 1;   //<sof1> as default

void ImuFrame_setSof(uint8_t val)
{
    sof = val;
}

uint8_t ImuFrame_size()
{
    if(sof==1) {    //<sof1>    1 : Euler Angles
        return IMU_FRAME_SIZE;
    }else if(sof==2){          //<sof2>    2 : Quaternion
        return IMU_QUATERNION_FRAME_SIZE;
    }else{
        CYASSERT(false);    //Never reach here
        return 0;
    }
}

void ImuFrame_init(ImuFrame *imu)
{
    memset(imu->data,0,IMU_QUATERNION_FRAME_SIZE);
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

uint16_t ImuFrame_checksum2(ImuFrame *imu,uint8_t imuFrameSize)
{
    uint16_t checksum = 0;
    for (uint8_t i = 0; i < (imuFrameSize-2); i++) checksum += (imu->data[i]);
    return checksum;
}

int ImuFrame_integrity2(ImuFrame *imu,uint8_t imuFrameSize)
{
    uint16_t checksum = ImuFrame_checksum2(imu,imuFrameSize);
    uint8_t high = (checksum&0xff00)>>8;
    uint8_t low = checksum&0xff;

    if(imu->data[imuFrameSize-2]!=high) return 0;
    if(imu->data[imuFrameSize-1]!=low) return 0;
    return 1;
}