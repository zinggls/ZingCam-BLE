#include "project.h"
#include "ai2c.h"

#define HIGH 0
#define LOW 1

int main(void)
{
    uint8_t data[2];
    uint8_t ret;
#if I2C_I2C_MASTER
    const uint8_t addr = 0x24;
#endif
    
    CyGlobalIntEnable;
    
    AI2C_init();
    
    while (1)
    {
#if I2C_I2C_MASTER
        ret = 0x01;
    
        if (AI2C_write(addr, &ret, 1) == 0)
        {
            if (AI2C_read(addr, data, 2) == 0)
            {
                if (data[0] == 0x01)
                {
                    if (data[1] == 1)
                    {
                        LED_RED_Write(HIGH);
                    }
                    else
                    {
                        LED_RED_Write(LOW);
                    }
                }
                else if (data[0] == 0x02)
                {
                    if (data[1] == 1)
                    {
                        LED_GREEN_Write(HIGH);
                    }
                    else
                    {
                        LED_GREEN_Write(LOW);
                    }
                }
                else if (data[0] == 0x03)
                {
                    if (data[1] == 1)
                    {
                        LED_BLUE_Write(HIGH);
                    }
                    else
                    {
                        LED_BLUE_Write(LOW);
                    }
                }
            }
        }
        CyDelay(1000);
#endif
#if I2C_I2C_SLAVE
        if (AI2C_read(data, 2) == 0)
        {
            if (data[0] == 0x01)
            {
                LED_RED_Write(LOW);
                
                if (data[1] == 1)
                {
                    LED_RED_Write(HIGH);
                    ret = 'O';
                    AI2C_write(&ret, 1);
                }
                else
                {
                    LED_RED_Write(LOW);
                    ret = 'X';
                    AI2C_write(&ret, 1);
                }
            }
            else if (data[0] == 0x02)
            {
                if (data[1] == 1)
                {
                    LED_GREEN_Write(HIGH);
                    ret = 'O';
                    AI2C_write(&ret, 1);
                }
                else
                {
                    LED_GREEN_Write(LOW);
                    ret = 'X';
                    AI2C_write(&ret, 1);
                }
            }
            else if (data[0] == 0x03)
            {
                if (data[1] == 1)
                {
                    LED_BLUE_Write(HIGH);
                    ret = 'O';
                    AI2C_write(&ret, 1);
                }
                else
                {
                    LED_BLUE_Write(LOW);
                    ret = 'X';
                    AI2C_write(&ret, 1);
                }
            }
        }
#endif
    }
}