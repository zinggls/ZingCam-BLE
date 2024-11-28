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
#include "ivf.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {0};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};

uint8 changeScope(ScopeCamera cam)
{
    uint8 result;

    /* Execute the received command */
    switch (cam)
    {
        case change_none:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            result = change_none;
            break;

        case change_eo:
            LED_RED_Write  (LED_ON);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_OFF);
            result = change_eo;
            break;

        case change_ir_white:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_ON);
            LED_BLUE_Write (LED_OFF);
            result = change_ir_white;
            break;

        case change_ir_black:
            LED_RED_Write  (LED_OFF);
            LED_GREEN_Write(LED_OFF);
            LED_BLUE_Write (LED_ON);
            result = change_ir_black;
            break;

        default:
            result = change_none;
            break;
    }

    return result;
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
            ivfCom.scopeCamera = changeScope(i2cWriteBuffer[0]);
            ivfCom.scopeOutput = i2cWriteBuffer[1];
            ivfCom.wirelessVideoChannelMode = i2cWriteBuffer[2];
        }
        
        /* Clear the slave write buffer and status */
        I2C_I2CSlaveClearWriteBuf();
        (void) I2C_I2CSlaveClearWriteStatus();
        
        /* Update the read buffer */
        setReadBuffer_ScopeCamera(&i2cReadBuffer[0],ivfCom.scopeCamera);
        setReadBuffer_ScopeOutput(&i2cReadBuffer[1],ivfCom.scopeOutput);
        i2cReadBuffer[2] = ivfCom.wirelessVideoChannelMode;
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
