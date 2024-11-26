/******************************************************************************
* File Name: main.c
*
* Version 1.00
*
* Description: 
*  This code example demonstrates basic usage of PSoC 4 Serial Communication 
*  Block (SCB) Component in I2C Slave mode. This project implements the 
*  I2C Slave device, which receives and executes commands from I2C Master and 
*  controls the RGB LED color. Result of command execution can be read by 
*  I2C Master. 
*
* Related Document: 
*  CE224599_I2C_Slave_using_a_Serial_Communication_Block(SCB)with_PSoC4.pdf
*
* Hardware Dependency: 
*  See CE224599_I2C_Slave_using_a_Serial_Communication_Block(SCB)with_PSoC4.pdf
*
*******************************************************************************
* Copyright (2018), Cypress Semiconductor Corporation. All rights reserved.
*******************************************************************************
* This software, including source code, documentation and related materials
* ("Software"), is owned by Cypress Semiconductor Corporation or one of its
* subsidiaries ("Cypress") and is protected by and subject to worldwide patent
* protection (United States and foreign), United States copyright laws and
* international treaty provisions. Therefore, you may use this Software only
* as provided in the license agreement accompanying the software package from
* which you obtained this Software ("EULA").
*
* If no EULA applies, Cypress hereby grants you a personal, non-exclusive,
* non-transferable license to copy, modify, and compile the Software source
* code solely for use in connection with Cypress’s integrated circuit products.
* Any reproduction, modification, translation, compilation, or representation
* of this Software except as specified above is prohibited without the express
* written permission of Cypress.
*
* Disclaimer: THIS SOFTWARE IS PROVIDED AS-IS, WITH NO WARRANTY OF ANY KIND, 
* EXPRESS OR IMPLIED, INCLUDING, BUT NOT LIMITED TO, NONINFRINGEMENT, IMPLIED 
* WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE. Cypress 
* reserves the right to make changes to the Software without notice. Cypress 
* does not assume any liability arising out of the application or use of the 
* Software or any product or circuit described in the Software. Cypress does 
* not authorize its products for use in any products where a malfunction or 
* failure of the Cypress product may reasonably be expected to result in 
* significant property damage, injury or death ("High Risk Product"). By 
* including Cypress’s product in a High Risk Product, the manufacturer of such 
* system or application assumes all risk of such use and in doing so agrees to 
* indemnify Cypress against all liability. 
*******************************************************************************/
#include <project.h>
#include "cyapicallbacks.h"

/* LED states */
#define LED_ON              (0xFFu)
#define LED_OFF             (0x00u)

/* Configure I2C packet and buffer sizes */
#define RD_BUFFER_SIZE      (0x03u)
#define WR_BUFFER_SIZE      (0x05u)
#define PACKET_SIZE         (WR_BUFFER_SIZE)

/* Configure the start and end of the I2C write packet */
#define PACKET_SOP          (0x01u)
#define PACKET_EOP          (0x17u)

/* Configure the position of each command in the packet */
#define PACKET_SOP_POS      (0x00u)
#define PACKET_STS_POS      (0x01u)
#define PACKET_RED_POS      (0x01u)
#define PACKET_GREEN_POS    (0x02u)
#define PACKET_BLUE_POS     (0x03u)
#define PACKET_EOP_POS      (0x04u)

/* Configure status values for read buffer */
#define STS_CMD_FAIL        (0xFFu)
#define STS_CMD_DONE        (0x00u)

/* Initialize the I2C buffers */
uint8 i2cReadBuffer [RD_BUFFER_SIZE] = {PACKET_SOP, STS_CMD_FAIL, PACKET_EOP};
uint8 i2cWriteBuffer[WR_BUFFER_SIZE];

/* Initialize error detection variable */
uint8 errorDetected = 0u;
uint8 I2CFlag = 0u;

/*******************************************************************************
* Function Name: I2CS_I2C_ISR_ExitCallback()
********************************************************************************
*
* Summary:
*   Interrupt Service Routine for I2CS_SCB_IRQ.
*   Sets a flag which is used to notify the CPU of new I2C data to process
* 
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void I2CS_I2C_ISR_ExitCallback()
{
    I2CFlag = 1;    
}

/*******************************************************************************
* Function Name: HandleError()
********************************************************************************
*
* Summary:
*       Loops infinitely
*       Disables interrupts
* 
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
void HandleError(void)
{
    /* Disable all Interrupts */
    CyGlobalIntDisable;
    
    /* infinite loop */
    while(1){}
}

/*******************************************************************************
* Function Name: main()
********************************************************************************
*
* Summary:
*       Initializes Components used in the design. 
*       Turns on the Green LED on Startup. 
*       Initializes I2C buffers for the Slave device. 
*       Enables Interrupts.
*       Checks for errors. 
* 
* Parameters:  
*   None
*
* Return:
*   None
*
*******************************************************************************/
int main(void)
{
    /* Initialize the PWM Components for LED control */
    PWM_Red_Start();
    PWM_Green_Start();
    PWM_Blue_Start();
    
    /* Turn on the green LED */
    PWM_Green_WriteCompare(LED_ON);
    PWM_Red_WriteCompare(LED_OFF);
    PWM_Blue_WriteCompare(LED_OFF);
    
    /* Set up the I2C Buffers and start the I2C Component */
    I2CS_I2CSlaveInitReadBuf(i2cReadBuffer, RD_BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cWriteBuffer, WR_BUFFER_SIZE);
    I2CS_Start();
    
    /* Interrupt handler */
    I2CS_I2C_ISR_ExitCallback();
    
    /* Enable global interrupts. */
    CyGlobalIntEnable; 

    for(;;)
    {
        
        if(I2CFlag != 0)
        {
            
            I2CFlag = 0;    
            
            /* Check if write is complete */
            if (0u != (I2CS_I2C_SSTAT_WR_CMPLT & I2CS_I2CSlaveStatus()))
            {
                
                /* Check the length of the packet */
                if (PACKET_SIZE == I2CS_I2CSlaveGetWriteBufSize())
                {
                    
                    /* Check the start and end of the packet */
                    if ((PACKET_EOP == i2cWriteBuffer[PACKET_EOP_POS]) &&
                        (PACKET_SOP == i2cWriteBuffer[PACKET_SOP_POS]))
                    {
                        /* Update the PWM compare values to control the RGB LED */
                        PWM_Red_WriteCompare(i2cWriteBuffer[PACKET_RED_POS]);
                        PWM_Green_WriteCompare(i2cWriteBuffer[PACKET_GREEN_POS]);
                        PWM_Blue_WriteCompare(i2cWriteBuffer[PACKET_BLUE_POS]);
                        i2cReadBuffer[PACKET_STS_POS] = STS_CMD_DONE;
                    }  
                    else /* Bad packet format, set error */
                    {
                        errorDetected = 1;
                    }    
                }
                /* If write complete but packet is invalid, set error */
                else
                {
                    errorDetected = 1;
                }
                
                /* Clear the slave write buffer for next write */
                I2CS_I2CSlaveClearWriteBuf();
                (void) I2CS_I2CSlaveClearWriteStatus();
            }
            
            /* Read Complete: Expose buffer to master */
            if(0u != (I2CS_I2C_SSTAT_RD_CMPLT & I2CS_I2CSlaveStatus()))
            {
                /* Clear the slave read buffer for next read */
                I2CS_I2CSlaveClearReadBuf();
                (void) I2CS_I2CSlaveClearReadStatus();
            }          
        }
        
        if(errorDetected == 1)
        {
            HandleError();
        }    

    }
}

/* [] END OF FILE */
