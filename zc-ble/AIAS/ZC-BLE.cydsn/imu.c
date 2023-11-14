#include  "imu.h"

void IMU_init(void)
{
    UART_IMU_Start();
}

uint8_t IMU_get(uint16_t* values)
{
    uint8_t upper;
    uint8_t lower;
    uint16_t SOP;
    uint16_t CHK;
    uint16_t DATA;
    uint8_t cnt;
    //char message[128];
    
    UART_IMU_UartPutChar('*');
    CyDelay(10);
    
    upper = UART_IMU_UartGetChar();
    lower = UART_IMU_UartGetChar();
    SOP = (upper << 8) | lower;
    CHK = 0;
    
    if (SOP == 0x5555)
    {
        cnt = 0;
        
        do
        {
            CHK = CHK + upper + lower;
            upper = UART_IMU_UartGetChar();
            lower = UART_IMU_UartGetChar();
            DATA = upper << 8 | lower;
            //sprintf(message, "%d. Receive %X (%X)\r\n", cnt, DATA, CHK);
            //UART_DBG_UartPutString(message);
            if (cnt <= NUM_TOTAL_IMU_VALUES)
            {
                values[cnt++] = DATA;
            }
            else
            {
                return 0;
            }
        } while (DATA != CHK);
                
        return 1;
    }
    else
    {
        return 0;
    }
}