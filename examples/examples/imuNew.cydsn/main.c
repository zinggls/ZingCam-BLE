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

#define IMU_FRAME_SIZE 12

static UartBuf uBuf;    //Circular buffer for UART data

CY_ISR(UART_IMU_RX_INTERRUPT)
{
    UartBuf_write_char(&uBuf,UART_IMU_GetChar());  // Write character to circular buffer

    // Clear the interrupt to prevent retriggering
    UART_IMU_RX_ClearInterrupt();
}

static char msg[128];

static char imu[IMU_FRAME_SIZE];
static int imuIndex = -1;
static bool imuEmpty = true;
static bool imuFull = false;

uint16_t calculate_checksum(char *data, int length) {
    uint16_t checksum = 0;
    for (int i = 0; i < length; i++) {
        checksum += data[i];
    }
    return checksum;
}

// Function to process data when a complete message is available
static void process_uart_data()
{
    while (!UartBuf_is_empty(&uBuf)) {
        char ch = UartBuf_read_char(&uBuf);
        if(imuEmpty) {
            if(ch!=0x55) continue;  //imu must start with 0x55
            
            imu[++imuIndex] = 0x55; //index=0
            imuEmpty = false;
        }else{
            if(imuIndex==0) {
                if(ch==0x55) {
                    imu[++imuIndex] = 0x55; //index=1
                }else{
                    //imu 2nd byte must be 0x55, reset
                    imuIndex = -1;
                    imuEmpty = true;
                    imuFull = false;
                    continue;
                }
            }else if(imuIndex>0 && imuIndex<11){
                imu[++imuIndex] = ch;
            }else{
                CYASSERT(imuIndex==(IMU_FRAME_SIZE-1));
                
                uint16_t checksum = calculate_checksum(imu,IMU_FRAME_SIZE-2);
                uint8_t high = (checksum&0xff00)>>8;
                uint8_t low = checksum&0xff;
                
                if(imu[IMU_FRAME_SIZE-2]==high && imu[IMU_FRAME_SIZE-1]==low) {
                    //valid checksum
                    imuFull = true;
                }else{
                    //invalid checksum
                    imuIndex = -1;
                    imuEmpty = true;
                    imuFull = false;
                    continue;
                }
            }
        }
        
        if(imuFull) {
            for(int i=0;i<IMU_FRAME_SIZE;i++) {
                sprintf(msg, "%X ", imu[i]);
                UART_DBG_UartPutString(msg);
            }
            sprintf(msg, "\r\n");
            UART_DBG_UartPutString(msg);

            imuIndex = -1;
            imuEmpty = true;
            imuFull = false;
        }
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UartBuf_init(&uBuf);
    
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
