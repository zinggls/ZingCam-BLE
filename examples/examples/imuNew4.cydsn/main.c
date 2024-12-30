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

static uint8 sof = 1;   //Euler angles as default
static uint8 imuCommand = 0;
static char msg[128];

static void process(uint8 command)
{
    if(command == 0) return;

    if(command==0x65) {         //<sof1>	SET OUTPUT FORMAT, 1:Euler Angles	'e'	0x65
        if(sof==1) {
            UART_DBG_UartPutString("skip <sof1>\r\n");  //already sof is 1
            goto cleanup;
        }
        
        sof = 1;
        ImuFrame_setSof(sof);
        UART_IMU_InitializeOutputFormat(sof);
    }else if(command==0x71) {   //<sof2>	SET OUTPUT FORMAT, 2:Quaternion	    'q'	0x71
        if(sof==2) {
            UART_DBG_UartPutString("skip <sof2>\r\n");  //already sof is 2
            goto cleanup;
        }
        
        sof = 2;
        ImuFrame_setSof(sof);
        UART_IMU_InitializeOutputFormat(sof);
    }else if(command==0x67) {   //<cg>      CALIBRATION GYRO, 자이로 보정	        'g'	0x67
        UART_IMU_UartPutString("<cg>");
        UART_DBG_UartPutString("<cg>\r\n");
    }else if(command==0x61) {   //<cas>	    CALIBRATION ACCELERO SIMPLE 가속도 보정	'a'	0x61
        UART_IMU_UartPutString("<cas>");
        UART_DBG_UartPutString("<cas>\r\n");
    }else if(command==0x6d) {   //<cmf>	    CALIBRATION MAGNETO FREE 지자계 보정	'm'	0x6d
        UART_IMU_UartPutString("<cmf>");
        UART_DBG_UartPutString("<cmf>\r\n");
    }else if(command==0x3e) {   //>	        cmf 종료명령	                        '>'	0x3e
        UART_IMU_UartPutString(">");
        UART_DBG_UartPutString(">\r\n");
    }else if(command==0x66) {   //<sem0>	지자계off  	                        'f'	0x66
        UART_IMU_UartPutString("<sem0>");
        UART_DBG_UartPutString("<sem0>\r\n");
    }else if(command==0x6f) {   //<sem1>	지자계off  	                        'o'	0x6f
        UART_IMU_UartPutString("<sem1>");
        UART_DBG_UartPutString("<sem1>\r\n");
    }else if(command==0x4d) {   //<sem2>	지자계능동형On  	                        'M'	0x4d
        UART_IMU_UartPutString("<sem2>");
        UART_DBG_UartPutString("<sem2>\r\n");
    }else{
        goto cleanup;
    }
    
    CyDelay(1000);
    
cleanup:
    imuCommand = 0;   //reset
}

static uint32 imuFrameCount = 0;

static void onImuFrame(const ImuFrame *imu)
{
    if(sof==1) UART_DBG_UartPutString("[Euler] ");
    if(sof==2) UART_DBG_UartPutString("[Quaternion] ");

    for(int i=0;i<ImuFrame_size();i++) {
        sprintf(msg, "%02X ", imu->data[i]);
        UART_DBG_UartPutString(msg);
    }
    sprintf(msg, "\r\n");
    UART_DBG_UartPutString(msg);
    imuFrameCount++;
}

CY_ISR_PROTO(UART_DBG_RX_ISR)
{
    // Check if data is received
    if (UART_DBG_SpiUartGetRxBufferSize() > 0)
    {
        // Read the received byte
        imuCommand = UART_DBG_UartGetChar();
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */    
    UART_DBG_Start();
    UART_DBG_SetCustomInterruptHandler(UART_DBG_RX_ISR);
    UART_IMU_StartAndInitialize();
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        imu_process_uart_data(onImuFrame);
        process(imuCommand);
    }
}

/* [] END OF FILE */
