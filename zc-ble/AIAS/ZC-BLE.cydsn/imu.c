#include  "imu.h"

void IMU_init(void)
{
    UART_IMU_Start();
}

uint8_t IMU_get(uint16_t* datas)
{
    uint8_t upper;
    uint8_t lower;
    uint16_t SOP;
    uint16_t CHK;
    uint16_t DATA;
    uint8_t cnt;
    
    UART_IMU_UartPutChar('*');
    
    upper = UART_IMU_UartGetChar();
    lower = UART_IMU_UartGetChar();
    SOP = (upper << 8) | lower;
    
    if (SOP == 0x5555)
    {
        cnt = 0;
        
        do
        {
            CHK = CHK + upper + lower;
            upper = UART_IMU_UartGetChar();
            lower = UART_IMU_UartGetChar();
            DATA = upper << 8 | lower;
            datas[cnt++] = DATA;
        } while (DATA != CHK);
        
        return 1;
    }
    else
    {
        return 0;
    }
}