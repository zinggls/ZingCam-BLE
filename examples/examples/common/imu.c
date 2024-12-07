#include "project.h"
#include "imu.h"

static UartBuf uBuf;    //Circular buffer for UART data
static ImuFrame imu;

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    UartBuf_write_char(&uBuf,UART_IMU_UartGetByte());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
}

void UART_IMU_StartAndInitialize()
{
    UartBuf_init(&uBuf);
    ImuFrame_init(&imu);
    
    UART_IMU_Start();
    
    UART_IMU_SetCustomInterruptHandler(UART_IMU_RX_INTERRUPT);    
    
    UART_IMU_UartPutString("<lf>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sor1>");
    CyDelay(100);
    UART_IMU_UartPutString("<soc2>");
    CyDelay(100);
    UART_IMU_UartPutString("<sots1>");
    CyDelay(100);
}

// Function to process data when a complete message is available
void imu_process_uart_data(ImuFrameCallback cb)
{
    while (!UartBuf_is_empty(&uBuf)) {
        char ch = UartBuf_read_char(&uBuf);
        if(imu.isEmpty) {
            if(ch!=0x55) continue;  //imu must start with 0x55
            
            imu.data[++imu.index] = 0x55; //index=0
            imu.isEmpty = false;
        }else{
            if(imu.index==0) {
                if(ch==0x55) {
                    imu.data[++imu.index] = 0x55; //index=1
                }else{
                    //imu 2nd byte must be 0x55, reset
                    ImuFrame_init(&imu);
                    continue;
                }
            }else if(imu.index>0 && imu.index<(IMU_FRAME_SIZE-1)){
                imu.data[++imu.index] = ch;
            }else{
                CYASSERT(imu.index==(IMU_FRAME_SIZE-1));
                
                if(ImuFrame_integrity(&imu)) {
                    //valid checksum
                    imu.isFull = true;
                }else{
                    ImuFrame_init(&imu);
                    continue;
                }
            }
        }
        
        if(imu.isFull) {
            cb(&imu);
            ImuFrame_init(&imu);
        }
    }
}

void setImuState(uint8 val, uint8 *buf)
{
    *buf = val;
}