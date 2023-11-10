#include <stdlib.h>

#include "zing.h"

void ZING_init(uint8_t** zing_device_status_values)
{
    UART_ZING_Start();
    
    zing_device_status_values = (uint8_t**)calloc(NUM_DEVICE_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
    {
        zing_device_status_values[i] = (uint8_t*)calloc(8, sizeof(uint8_t));
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
                device_status[cnt++] = ch;
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

void ZING_parse_device_status(char* device_status, uint8_t** values)
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
            memcpy(values[idx++], status, strlen(status));
        }
        cnt = cnt + 1;
    }
}