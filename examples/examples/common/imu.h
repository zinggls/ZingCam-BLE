#ifndef IMU_H
#define IMU_H
    
#include "UartBuf.h"
#include "ImuFrame.h"
    
typedef void (*ImuFrameCallback)(const ImuFrame *imu);

UartBuf uBuf;    //Circular buffer for UART data
ImuFrame imu;

void UART_IMU_StartAndInitialize();
void process_uart_data(ImuFrameCallback cb);
    
#endif //IMU_H