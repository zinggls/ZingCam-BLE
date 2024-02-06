#include "i2c.h"
#include "main.h"

void I2C_init(void)
{
    I2C_Start();
}

uint8_t I2C_write(uint8_t addr, uint8_t* data, uint8_t len)
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
    // add whenever ~_~
#endif
    return status;
}