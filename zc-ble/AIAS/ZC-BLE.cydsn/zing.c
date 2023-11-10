#include <stdlib.h>

#include "zing.h"

uint8_t** ZING_host_init(void)
{
    uint8_t** status_values;
    
    UART_ZING_Start();
    
    status_values = (uint8_t**)calloc(NUM_HOST_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
    {
        status_values[i] = (uint8_t*)calloc(MAX_VALUE_LENGTH, sizeof(uint8_t));
    }
    
    return status_values;
}

uint8_t** ZING_device_init(void)
{
    uint8_t** status_values;
    
    UART_ZING_Start();
        
    status_values = (uint8_t**)calloc(NUM_DEVICE_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
    {
        status_values[i] = (uint8_t*)calloc(MAX_VALUE_LENGTH, sizeof(uint8_t));
    }
    
    return status_values;
}

uint8_t ZING_get_host_status(char* host_status)
{
    char ch;
    uint16_t cnt;
    uint16_t loop;
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    loop = 0;
    
    if (ch == ASCII_HOST)
    {
        host_status[cnt++] = ASCII_HOST;
        
        do
        {
            if ((ch = UART_ZING_GetChar()) != 0)
            {
                if (cnt < MAX_BUFFER_LENGTH)
                {
                    host_status[cnt++] = ch;
                }
                else
                {
                    return 0;
                }
            }
        }
        while (ch != ASCII_LF);
        
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ZING_get_device_status(char* device_status)
{
    char ch;
    uint16_t cnt;
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    
    if (ch == ASCII_DEVICE)
    {
        device_status[cnt++] = ASCII_DEVICE;
        
        do
        {
            if ((ch = UART_ZING_GetChar()) != 0)
            {
                if (cnt < MAX_BUFFER_LENGTH)
                {
                    device_status[cnt++] = ch;
                }
                else
                {
                    return 0;
                }
            }
        }
        while (ch != ASCII_LF);
        
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ZING_parse_host_status(char* host_status, uint8_t** status_values)
{
    char* status = strtok(host_status, " ");
    uint8_t cnt;
    uint8_t idx;
    
    char message[128];
    
    cnt = 0;
    idx = 0;
    
    while ((status = strtok(NULL, " :")) != NULL)
    {
        if ((cnt % 2) == 1)
        {
            if (idx < NUM_HOST_STATUS)
            {
                if (strlen(status) < MAX_VALUE_LENGTH)
                {                   
                    memset(status_values[idx], 0, MAX_VALUE_LENGTH);
                    memcpy(status_values[idx++], status, strlen(status));
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        
        cnt = cnt + 1;
    }
    
    return 1;
}

void ZING_parse_device_status(char* device_status, uint8_t** status_values)
{
    char* status = strtok(device_status, " ");
    uint8_t cnt;
    uint8_t idx;
    
    cnt = 0;
    idx = 0;
    
    while ((status = strtok(NULL, " :")) != NULL)
    {
        if ((cnt % 2) == 1)
        {
            memcpy(status_values[idx++], status, strlen(status));
        }
        cnt = cnt + 1;
    }
}