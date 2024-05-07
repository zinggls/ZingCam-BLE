#include "../headers/imu.h"
#include "main.h"

void IMU_init(void)
{
    UART_IMU_Start();
    UART_IMU_UartPutString("<sots1>");
    UART_IMU_UartPutString("<soc1>");
}

uint8_t IMU_recv(char* imu)
{
    uint8_t ch;
    uint32_t tick;
    
    tick = AIAS_get_systick();
    
    ch = UART_IMU_UartGetChar();
    
    if (ch != '*')
    {
        return 0;
    }
    
    for (uint8_t i = 0; i < MAX_IMU_RECV_LENGTH; i++)
    {
        ch = UART_IMU_UartGetChar();
        
        if (ch == 0 && i > 0)
        {
            if (AIAS_get_systick() - tick > MAX_IMU_TIMEOUT_MS)
            {
                return 0;
            }
            else
            {
                i =  i - 1;
                continue;
            }
        }
        
        imu[i] = ch;
        
        if (ch == '\n')
        {
            break;
        }
    }
    return 1;
}

uint8_t IMU_parse(char* imu, char** imu_values, uint8_t type)
{
    char* value;
    char* next_value;
    uint8_t idx;
    
    value = strtok_r(imu, "*,", &next_value);
    idx = 0;
    
    if (value[0] == '*')
    {
        return 0;
    }
    
    while (value != NULL)
    {
        if (type == 0)
        {
            if (idx < NUM_EULER_IMU)
            {
                strcpy(imu_values[idx], value);        
                value = strtok_r(NULL, ",", &next_value);
                idx = idx + 1;
            }
            else
            {
                return 0;
            }
        }
        else 
        {
            if (idx < NUM_QUATERNION_IMU)
            {
                strcpy(imu_values[idx], value);        
                value = strtok_r(NULL, ",", &next_value);
                idx = idx + 1;
            }
            else
            {
                return 0;
            }
        }
    }
    
    return 1;
}

IMU_VALUE IMU_parse_value(char* imu_values)
{
    char* upper;
    char* lower;
    IMU_VALUE ret;
    
    if (imu_values[0] == '-')
    {
        ret.neg = 1;
        upper = strtok_r(imu_values + 1, ".", &lower);
    }
    else
    {
        ret.neg = 0;
        upper = strtok_r(imu_values, ".", &lower);
    }
    
    ret.upper = strtol(upper, NULL, 16);
    ret.lower = strtol(lower, NULL, 16);
    
    return ret;
}