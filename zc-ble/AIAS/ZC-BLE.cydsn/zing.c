#include <stdlib.h>

#include "zing.h"

static uint8_t state = 0;

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
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    
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
            ch = UART_ZING_GetChar();
            if (ch != 0)
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
        while (ch != '\n');
        
        return 1;
    }
    else
    {
        return 0;
    }
}

uint8_t ZING_parse_host_status(char* host_status, uint8_t** status_values)
{
    char* status;
    char* next_ptr;
    uint8_t cnt;
    uint8_t idx;
    
    cnt = 0;
    idx = 0;
    
    status = strtok_r(host_status, " :", &next_ptr);
    while ((status = strtok_r(NULL, " :", &next_ptr)) != NULL)
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

uint8_t ZING_parse_device_status(char* device_status, uint8_t** status_values)
{
    char* status;
    char* next_ptr;
    uint8_t cnt;
    uint8_t idx;
    
    cnt = 0;
    idx = 0;
    
    status = strtok_r(device_status, " :", &next_ptr);
    while ((status = strtok_r(NULL, " :", &next_ptr)) != NULL)
    {
        if ((cnt % 2) == 1)
        {
            if (idx < NUM_DEVICE_STATUS)
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

void ZING_change_channel(uint8_t** host_status, uint8_t val)
{
    if (host_status == NULL)
    {
        // val = 0: state no change
        // val = 1: state need change
        state = val;
    }
    else
    {
        if (state == 1)
        {
            if (strcmp((char*)host_status[HOST_STATUS_BND], "L") == 0)
            {   
                UART_ZING_PutChar(0x4);
                UART_ZING_PutChar('b');
                UART_ZING_PutChar(0x1);
                UART_ZING_PutChar(0x0);
            }
            else if (strcmp((char*)host_status[HOST_STATUS_BND], "H") == 0)
            {
                UART_ZING_PutChar(0x4);
                UART_ZING_PutChar('b');
                UART_ZING_PutChar(0x0);
                UART_ZING_PutChar(0x0);
            }
            
            state = 0;
        }
    }
}