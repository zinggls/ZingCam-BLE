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

#include "i2cs.h"
#include "project.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {PACKET_SOP, STS_CMD_FAIL, PACKET_EOP};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};
static uint8 status = STS_CMD_FAIL;

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

void i2cs_start(void)
{
    I2C_I2CSlaveInitReadBuf (i2cReadBuffer,  I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, I2C_WR_BUFFER_SIZE);
    I2C_Start();
}

void i2cs_process(void)
{
    /* Write complete: parse the command packet */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
    {
        /* Check the packet length */
        if (PACKET_SIZE == I2C_I2CSlaveGetWriteBufSize())
        {
            /* Check the start and end of packet markers */
            if ((i2cWriteBuffer[PACKET_SOP_POS] == PACKET_SOP) &&
                (i2cWriteBuffer[PACKET_EOP_POS] == PACKET_EOP))
            {
                status = ExecuteCommand(i2cWriteBuffer[PACKET_CMD_POS]);
            }
        }
        
        /* Clear the slave write buffer and status */
        I2C_I2CSlaveClearWriteBuf();
        (void) I2C_I2CSlaveClearWriteStatus();
        
        /* Update the read buffer */
        i2cReadBuffer[PACKET_STS_POS] = status;
        status = STS_CMD_FAIL;
    }
    
    /* Read complete: expose buffer to master */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
    {
        /* Clear the slave read buffer and status */
        I2C_I2CSlaveClearReadBuf();
        (void) I2C_I2CSlaveClearReadStatus();
    }
}

/* [] END OF FILE */
