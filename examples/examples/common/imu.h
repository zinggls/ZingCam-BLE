#ifndef IMU_H
#define IMU_H
    
#include <project.h>
#include "UartBuf.h"
#include "ImuFrame.h"
    
typedef void (*ImuFrameCallback)(const ImuFrame *imu);

void UART_IMU_StartAndInitialize();
void imu_process_uart_data(ImuFrameCallback cb);
void setImuState(uint8 val, uint8 errCode, uint8 *buf);
uint8 getImuState();
    
#endif //IMU_H