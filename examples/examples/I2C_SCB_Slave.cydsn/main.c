/******************************************************************************
* File Name: main.c
*
* Version 1.30
*
* Description: 
*  This code example demonstrates basic usage of PSoC 4 Serial Communication 
*  Block (SCB) Component in I2C Slave mode. This project implements the 
*  I2C Slave device, which receives and executes commands from I2C Master and 
*  controls the RGB LED color. Result of command execution can be read by 
*  I2C Master. 
*
* Related Document: 
*  CE222306_PSoC4_I2C_SCB.pdf
*
* Hardware Dependency: 
*  See CE222306_PSoC4_I2C_SCB.pdf
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

#include <main.h>

/* The I2C Slave read and write buffers */
uint8 i2cReadBuffer [BUFFER_SIZE] = {PACKET_SOP, STS_CMD_FAIL, PACKET_EOP};
uint8 i2cWriteBuffer[BUFFER_SIZE];


/*******************************************************************************
* Function Name: ExecuteCommand
********************************************************************************
* 
* Summary:
*  Executes received command to control the LED color and returns status.
*  If the command is unknown, the LED color is not changed.
*
* Parameters:
*  cmd: command to execute. Supported commands:
*   - CMD_SET_RED:   set red color of the LED.
*   - CMD_SET_GREEN: set green color of the LED.
*   - CMD_SET_BLUE:  set blue color of the LED.
*   - CMD_SET_OFF:   turn off the LED.
*
* Return: 
*  Status of the command execution. There are two statuses
*  - STS_CMD_DONE: command is executed successfully.
*  - STS_CMD_FAIL: unknown command
*
*******************************************************************************/
uint8 ExecuteCommand(uint32 cmd)
{
    uint8 status;

    status = STS_CMD_DONE;

    /* Execute the received command */
    switch (cmd)
    {
        case CMD_SET_RED:
            LED_RED_Write  (LED_ON);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            break;

        case CMD_SET_GREEN:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_ON);
            LED_BLUE_Write (LED_OFF);
            break;

        case CMD_SET_BLUE:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_ON);
            break;

        case CMD_SET_OFF:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            break;

        default:
            status = STS_CMD_FAIL;
        break;
    }

    return(status);
}


/*******************************************************************************
* Function Name: main
********************************************************************************
* 
* Summary:
*  This function performs following actions:
*   1. Turns on the green LED.
*   2. Enables global interrupts.
*   3. Sets up the I2C Slave write and read buffers.
*   4. Starts the I2C Slave Component.
*   5. Waits for the command from the I2C Master to control the RGB LED.
*
*******************************************************************************/
int main()
{
    uint8 status = STS_CMD_FAIL;
    
    /* Turn on the green LED */
    (void) ExecuteCommand(CMD_SET_GREEN);
    
    CyGlobalIntEnable;
    
    /* Start the I2C Slave */
    I2CS_I2CSlaveInitReadBuf (i2cReadBuffer,  BUFFER_SIZE);
    I2CS_I2CSlaveInitWriteBuf(i2cWriteBuffer, BUFFER_SIZE);
    I2CS_Start();
    
    for (;;)
    {
        /* Write complete: parse the command packet */
        if (0u != (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_WR_CMPLT))
        {
            /* Check the packet length */
            if (PACKET_SIZE == I2CS_I2CSlaveGetWriteBufSize())
            {
                /* Check the start and end of packet markers */
                if ((i2cWriteBuffer[PACKET_SOP_POS] == PACKET_SOP) &&
                    (i2cWriteBuffer[PACKET_EOP_POS] == PACKET_EOP))
                {
                    status = ExecuteCommand(i2cWriteBuffer[PACKET_CMD_POS]);
                }
            }
            
            /* Clear the slave write buffer and status */
            I2CS_I2CSlaveClearWriteBuf();
            (void) I2CS_I2CSlaveClearWriteStatus();
            
            /* Update the read buffer */
            i2cReadBuffer[PACKET_STS_POS] = status;
            status = STS_CMD_FAIL;
        }
        
        /* Read complete: expose buffer to master */
        if (0u != (I2CS_I2CSlaveStatus() & I2CS_I2C_SSTAT_RD_CMPLT))
        {
            /* Clear the slave read buffer and status */
            I2CS_I2CSlaveClearReadBuf();
            (void) I2CS_I2CSlaveClearReadStatus();
        }
    }
}


/* [] END OF FILE */
