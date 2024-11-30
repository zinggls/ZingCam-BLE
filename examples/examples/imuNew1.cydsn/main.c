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
#include "imu.h"

static char msg[128];

static void onImuFrame(const ImuFrame *imu)
{
    for(int i=0;i<IMU_FRAME_SIZE;i++) {
        sprintf(msg, "%X ", imu->data[i]);
        UART_DBG_UartPutString(msg);
    }
    sprintf(msg, "\r\n");
    UART_DBG_UartPutString(msg);
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
