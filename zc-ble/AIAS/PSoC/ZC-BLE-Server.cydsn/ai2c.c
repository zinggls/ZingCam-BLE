#include "ai2c.h"
#include "main.h"
#include "icd.h"

#define I2C_WR_BUFFER_SIZE NUM_WRITE_AIAS_ICD
#define I2C_RD_BUFFER_SIZE NUM_READ_AIAS_ICD
    
static uint8_t i2c_slave_wr_buffer[I2C_WR_BUFFER_SIZE];
static uint8_t i2c_slave_rd_buffer[I2C_RD_BUFFER_SIZE];

void AI2C_init(void)
{
    I2C_I2CSlaveInitReadBuf(i2c_slave_rd_buffer, I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2c_slave_wr_buffer, I2C_WR_BUFFER_SIZE);
    I2C_Start();
}

uint8_t AI2C_write(void)
{
    uint8_t status;
    
    status = 0;
    
    if ((I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_WR_CMPLT) != 0)
    {    
        if (I2C_I2CSlaveGetWriteBufSize() == I2C_WR_BUFFER_SIZE)
        {
            status = 1;
        }
        else
        {
            status = 0;
        }
        
        I2C_I2CSlaveClearWriteBuf();
        I2C_I2CSlaveClearWriteStatus();
    }
    
    return status;
}

uint8_t AI2C_read(void)
{
    uint8_t status;
    
    status = 0;
    
    if ((I2C_I2CSlaveStatus() & I2C_I2C_SSTAT_RD_CMPLT) != 0)
    {
        if (I2C_I2CSlaveGetReadBufSize() == I2C_RD_BUFFER_SIZE)
        {
            status = 1;   
        }
        else
        {
            status = 0;
        }
        
        I2C_I2CSlaveClearReadBuf();
        I2C_I2CSlaveClearReadStatus();
    }
    
    return status;
}

void AI2C_set_read_buffer(uint8_t* buffer, uint8_t len)
{
    memcpy(i2c_slave_rd_buffer, buffer, len);
}

void AI2C_set_write_buffer(uint8_t* buffer, uint8_t len)
{
    memcpy(buffer, i2c_slave_wr_buffer, len);
}