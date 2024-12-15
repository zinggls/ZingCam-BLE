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
#include "Peripheral.h"

static uint8 i2cReadBuffer [I2C_RD_BUFFER_SIZE] = {0};
static uint8 i2cWriteBuffer[I2C_WR_BUFFER_SIZE] = {0};
static uint32 notifyCustom = 0;

void i2cs_start(void)
{
    I2C_I2CSlaveInitReadBuf (i2cReadBuffer,  I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2cWriteBuffer, I2C_WR_BUFFER_SIZE);
    I2C_Start();
}

void i2c_command_to_bcc()
{
    CYBLE_GATTS_HANDLE_VALUE_NTF_T myDataHandle;
    myDataHandle.attrHandle = CYBLE_CUSTOM_SERVICE_ZXX_CHAR_HANDLE;
    myDataHandle.value.val = (uint8_t *)&peripheral;
    myDataHandle.value.len = sizeof(PERIPHERAL);
    CyBle_GattsWriteAttributeValue( &myDataHandle, 0, &cyBle_connHandle, 0 );
    if(CyBle_GattsNotification(cyBle_connHandle,&myDataHandle)==CYBLE_ERROR_OK) notifyCustom++;
}

void i2cs_process()
{
    /* Write complete: parse the command packet */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT))
    {
        /* Check the packet length */
        if (PACKET_SIZE == I2C_I2CSlaveGetWriteBufSize())
        {
            peripheral.scope.scopeKindChangeNotify = i2cWriteBuffer[0];
            peripheral.scope.scopeOutChangeNotify = i2cWriteBuffer[1];
            peripheral.scope.scopeOperMode = i2cWriteBuffer[2];
            peripheral.scope.scopeStateKind = i2cWriteBuffer[3];
            peripheral.scope.scopeStateOut = i2cWriteBuffer[4];
            peripheral.scope.scopeStateBattery = i2cWriteBuffer[5];
            peripheral.scope.scopeStateIR = i2cWriteBuffer[6];
            peripheral.scope.scopeStateEO = i2cWriteBuffer[7];
            
            i2c_command_to_bcc();
        }
        
        /* Clear the slave write buffer and status */
        I2C_I2CSlaveClearWriteBuf();
        (void) I2C_I2CSlaveClearWriteStatus();
        
        /* Update the read buffer */
        i2cReadBuffer[0] = peripheral.scope.scopeKindChangeNotify;
        i2cReadBuffer[1] = peripheral.scope.scopeOutChangeNotify;
        i2cReadBuffer[2] = peripheral.scope.scopeOperMode;
    }
    
    /* Read complete: expose buffer to master */
    if (0u != (I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT))
    {
        /* Clear the slave read buffer and status */
        I2C_I2CSlaveClearReadBuf();
        (void) I2C_I2CSlaveClearReadStatus();
    }
}

uint8_t* getI2CReadBuffer()
{
    return i2cReadBuffer;
}

/* [] END OF FILE */
