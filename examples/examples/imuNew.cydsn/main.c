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

static UartBuf uBuf;    //Circular buffer for UART data

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    UartBuf_write_char(&uBuf,UART_IMU_GetChar());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
}

static char msg[128];

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

static Imu imu;

// Function to process data when a complete message is available
static void process_uart_data()
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
                    Imu_init(&imu);
                    continue;
                }
            }else if(imu.index>0 && imu.index<(IMU_FRAME_SIZE-1)){
                imu.data[++imu.index] = ch;
            }else{
                CYASSERT(imu.index==(IMU_FRAME_SIZE-1));
                
                if(Imu_integrity(&imu)) {
                    //valid checksum
                    imu.isFull = true;
                }else{
                    Imu_init(&imu);
                    continue;
                }
            }
        }
        
        if(imu.isFull) {
            for(int i=0;i<IMU_FRAME_SIZE;i++) {
                sprintf(msg, "%X ", imu.data[i]);
                UART_DBG_UartPutString(msg);
            }
            sprintf(msg, "\r\n");
            UART_DBG_UartPutString(msg);
            
            Imu_init(&imu);
        }
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UartBuf_init(&uBuf);
    Imu_init(&imu);
    
    UART_DBG_Start();
    UART_IMU_Start();
    
    UART_IMU_RX_INTR_StartEx(UART_IMU_RX_INTERRUPT);    
    
    UART_IMU_PutString("<lf>");
    CyDelay(1000);
    UART_IMU_PutString("<sor10>");
    CyDelay(100);
    UART_IMU_PutString("<soc2>");
    CyDelay(100);
    UART_IMU_PutString("<sots1>");
    CyDelay(100);
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        process_uart_data();
    }
}

/* [] END OF FILE */
