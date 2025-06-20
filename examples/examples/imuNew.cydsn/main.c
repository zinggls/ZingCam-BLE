/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <UartBuf.h>
#include <ImuFrame.h>

typedef void (*ImuFrameCallback)(const ImuFrame *imu);

static UartBuf uBuf;    //Circular buffer for UART data
static char msg[128];
static ImuFrame imu;

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    UartBuf_write_char(&uBuf,UART_IMU_GetChar());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
}

static void onImuFrame(const ImuFrame *imu)
{
    for(int i=0;i<IMU_FRAME_SIZE;i++) {
        sprintf(msg, "%X ", imu->data[i]);
        UART_DBG_UartPutString(msg);
    }
    sprintf(msg, "\r\n");
    UART_DBG_UartPutString(msg);
}

// Function to process data when a complete message is available
static void process_uart_data(ImuFrameCallback cb)
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

static void UART_IMU_StartAndInitialize()
{
    UartBuf_init(&uBuf);
    ImuFrame_init(&imu);
    
    UART_IMU_Start();
    
    UART_IMU_RX_INTR_StartEx(UART_IMU_RX_INTERRUPT);    
    
    UART_IMU_PutString("<lf>");
    CyDelay(1000);
    UART_IMU_PutString("<sor1>");
    CyDelay(100);
    UART_IMU_PutString("<soc2>");
    CyDelay(100);
    UART_IMU_PutString("<sots1>");
    CyDelay(100);
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */    
    UART_DBG_Start();
    UART_IMU_StartAndInitialize();
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        process_uart_data(onImuFrame);
    }
}

/* [] END OF FILE */
