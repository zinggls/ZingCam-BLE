#include "ai2c.h"
#include "main.h"
#include "icd.h"

#if DBLE
#define I2C_WR_BUFFER_SIZE NUM_WRITE_AIAS_ICD
#define I2C_RD_BUFFER_SIZE NUM_READ_AIAS_ICD
    
static uint8_t i2c_slave_wr_buffer[I2C_WR_BUFFER_SIZE];
static uint8_t i2c_slave_rd_buffer[I2C_RD_BUFFER_SIZE];
#endif

void AI2C_init(void)
{
#if DBLE
    I2C_I2CSlaveInitReadBuf(i2c_slave_rd_buffer, I2C_RD_BUFFER_SIZE);
    I2C_I2CSlaveInitWriteBuf(i2c_slave_wr_buffer, I2C_WR_BUFFER_SIZE);
#endif
    I2C_Start();
}

#if HBLE
uint8_t AI2C_write(uint8_t addr, uint8_t* data, uint8_t len)
#endif
#if DBLE
uint8_t AI2C_write(void)
#endif
{
    uint8_t status;
#if HBLE
    I2C_I2CMasterClearStatus();
    
    if ((status = I2C_I2CMasterWriteBuf(addr, data, len, I2C_I2C_MODE_COMPLETE_XFER)) == I2C_I2C_MSTR_NO_ERROR)
    {
        while (((status = I2C_I2CMasterStatus()) & I2C_I2C_MSTAT_WR_CMPLT) == 0)
        {
            // wait until transfer complete.
            // need to add timeout
        }
        
        if (((status = I2C_I2CMasterStatus()) & I2C_I2C_MSTAT_ERR_XFER) == 0)
        {
            if (I2C_I2CMasterGetWriteBufSize() == len)
            {
                return 0;
            }
            else
            {
                return status;
            }
        }
    }
    
#endif
#if DBLE
    if (((status = I2C_I2CSlaveStatus()) & I2C_I2C_SSTAT_WR_CMPLT) != 0)
    {    
        I2C_I2CSlaveClearWriteBuf();
        I2C_I2CSlaveClearWriteStatus();
        
        if (I2C_I2CSlaveGetWriteBufSize() == I2C_WR_BUFFER_SIZE)
        {
            return 1;   
        }
        
    }
    else
    {
        return status;
    }
#endif
    return 0;
}

#if HBLE
uint8_t AI2C_read(uint8_t addr, uint8_t* data, uint8_t len)
#endif
#if DBLE
uint8_t AI2C_read(void)
#endif
{
    uint8_t status;
#if HBLE
    I2C_I2CMasterClearStatus();
    
    if ((status = I2C_I2CMasterReadBuf(addr, data, len, I2C_I2C_MODE_COMPLETE_XFER)) == I2C_I2C_MSTR_NO_ERROR)
    {
        while (((status = I2C_I2CMasterStatus()) & I2C_I2C_MSTAT_RD_CMPLT) == 0)
        {
            // wait until transfer complete.
            // need to add timeout
        }
        
        if (((status = I2C_I2CMasterStatus()) & I2C_I2C_MSTAT_ERR_XFER) == 0)
        {
            if (I2C_I2CMasterGetReadBufSize() == len)
            {
                return 0;
            }
            else
            {
                return status;
            }
        }
    }
#endif
#if DBLE
    if (((status = I2C_I2CSlaveStatus()) & I2C_I2C_SSTAT_RD_CMPLT) != 0)
    {
        I2C_I2CSlaveClearReadBuf();
        I2C_I2CSlaveClearReadStatus();
        
        if (I2C_I2CSlaveGetReadBufSize() == I2C_RD_BUFFER_SIZE)
        {
            return 0;    
        }
        
    }
    else
    {
        return status;
    }
#endif
    return 0;
}

#if DBLE
void AI2C_set_read_buffer(uint8_t* buffer, uint8_t len)
{
    memcpy(i2c_slave_rd_buffer, buffer, len);
}

void AI2C_set_write_buffer(uint8_t* buffer, uint8_t len)
{
    memcpy(buffer, i2c_slave_wr_buffer, len);
}
#endif