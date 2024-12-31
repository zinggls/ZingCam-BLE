#include "project.h"
#include "imu.h"

static UartBuf uBuf;    //Circular buffer for UART data
static ImuFrame imu;
static uint16 cbCountPrev,cbCount;

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    uint32_t interruptState = CyEnterCriticalSection();
    
    UartBuf_write_char(&uBuf,UART_IMU_UartGetByte());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
    
    CyExitCriticalSection(interruptState);
}

void UART_IMU_StartAndInitialize()
{
    UART_IMU_Start();
    UART_IMU_SetCustomInterruptHandler(UART_IMU_RX_INTERRUPT);
    UART_IMU_InitializeOutputFormat(1); //sof 1
}

void UART_IMU_InitializeOutputFormat(uint8 sof)
{
    UartBuf_init(&uBuf);
    ImuFrame_init(&imu);
    
    UART_IMU_UartPutString("<lf>");
    CyDelay(1000);
    UART_IMU_UartPutString("<sor10>");
    CyDelay(100);
    UART_IMU_UartPutString("<soc2>");
    CyDelay(100);
    UART_IMU_UartPutString("<sots1>");
    CyDelay(100);
    
    if(sof==1) UART_IMU_UartPutString("<sof1>");
    if(sof==2) UART_IMU_UartPutString("<sof2>");
    
    cbCountPrev = cbCount = 0;
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
            }else if(imu.index>0 && imu.index<(ImuFrame_size()-1)){
                imu.data[++imu.index] = ch;
            }else{
                CYASSERT(imu.index==(ImuFrame_size()-1));
                
                if(ImuFrame_integrity2(&imu,ImuFrame_size())) {
                    //valid checksum
                    imu.isFull = true;
                }else{
                    ImuFrame_init(&imu);
                    continue;
                }
            }
        }
        
        if(imu.isFull) {
            cb(&imu); cbCount++;
            ImuFrame_init(&imu);
        }
    }
}

void setImuState(uint8 val, uint8 errCode, uint8 *buf)
{
    if(val==0) {
        *buf = val;
    }else{
        *buf = errCode;
    }
}

uint8 getImuState()
{
    if(cbCount==cbCountPrev) {
        return 1;   //ImuFrameCallback 호출 횟수 변화 없음
    }else{
        cbCountPrev = cbCount;
        return 0;   //ImuFrameCallback 호출 횟수 변화 있음
    }
}