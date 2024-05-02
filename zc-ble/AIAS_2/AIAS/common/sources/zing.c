#include "../headers/zing.h"
#include "../headers/main.h"

static uint8_t ZCH_STATUS_type[] =
{
    // NUMERIC or HEX   = 0
    // ASCII_BND        = 1
    // ASCII_TRT        = 2
    // ASCII_ACK        = 3
    // ASCII_PPC        = 4
    // ASCII_RUN        = 5
    0, // USB
    0, // VND
    0, // PRD
    1, // BND
    0, // PPID
    0, // DeviceID
    0, // FMT
    0, // IDX
    2, // TRT
    3, // ACK
    4, // PPC
    0, // TXID
    0, // RXID
    5, // RUN
    0, // CNT
};

static uint8_t ZCD_STATUS_type[] =
{
    // NUMERIC or HEX   = 0
    // ASCII_TRT        = 2
    // ASCII_ACK        = 3
    // ASCII_PPC        = 4
    // ASCII_RUN        = 5
    0, // USB
    0, // PPID
    0, // DeviceID
    0, // FMT
    0, // FPS
    0, // IDX
    2, // TRT
    3, // ACK
    4, // PPC
    5, // RUN
    6, // ITF
    0, // TXID
    0, // RXID
    0, // DestID_ERR_CNT
    0, // PHY_RX_FRAME_CNT
    0, // MFIR
    0, // CNT
};

static uint8_t ZING_get_zch_status_type(uint8_t idx, char value);
static uint8_t ZING_get_zcd_status_type(uint8_t idx, char value);

void ZING_init(void)
{
    UART_ZING_Start();
}

uint8_t ZING_get_status(char* zing_status)
{
    char ch;
    uint32_t tick;
    
    tick = ZCBLE_get_systick();
    
    for (uint8_t i = 0; i < MAX_ZING_STATUS_LENGTH; i++)
    {   
        ch = UART_ZING_GetChar();
        
        if (ch == 0 && i > 0)
        {
            if (ZCBLE_get_systick() - tick > MAX_ZING_TIMEOUT_MS)
            {
                return 0;
            }
            else
            {
                i =  i - 1;
                continue;
            }
        }
        
        if (ch == 'Z')
        {
            i = 0;
        }
        
        zing_status[i] = ch;
        
        if (ch == '\n')
        {
            break;
        }
    }
    
    return 1;
}

uint8_t ZING_parse_zch_status(char* zing_status, char** status_values)
{
    char* elem;
    char* next_elem;
    uint8_t idx;
    int num;
    char msg[128];
    uint32_t tick;
    
    tick = ZCBLE_get_systick();
    elem = strtok_r(zing_status, " ", &next_elem);
    idx = 0;
    
    if (strcmp(elem, "ZCH") != 0)
    {
        return 0;
    }
    
    while (elem != NULL)
    {
        if (ZCBLE_get_systick() - tick > MAX_ZING_TIMEOUT_MS)
        {
            return 0;
        }
        
        if (idx < NUM_ZCH_STATUS)
        {
            char* name;
            char* value;
            
            elem = strtok_r(NULL, " ", &next_elem);
            strcpy(status_values[idx], elem);
            name = strtok_r(status_values[idx], ":", &value);
            
            if (ZCH_STATUS_type[idx] == 0)
            {
                num = strtol(value, NULL, 16);
            }
            else
            {
                num = ZING_get_zch_status_type(idx, value[0]);
            }
            sprintf(msg, "%s %X\r\n", name, num);
            UART_DBG_UartPutString(msg);
            idx = idx + 1;
        }
        else
        {
            return 1;
        }
    }
    
    return 1;
}

uint8_t ZING_parse_zcd_status(char* zing_status, char** status_values)
{
    char* elem;
    char* next_elem;
    uint8_t idx;
    int num;
    char msg[128];
    uint32_t tick;
    
    tick = ZCBLE_get_systick();
    elem = strtok_r(zing_status, " ", &next_elem);
    idx = 0;
    
    if (strcmp(elem, "ZCD") != 0)
    {
        return 0;
    }
    
    while (elem != NULL)
    {
        if (ZCBLE_get_systick() - tick > MAX_ZING_TIMEOUT_MS)
        {
            return 0;
        }
        
        if (idx < NUM_ZCD_STATUS)
        {
            char* name;
            char* value;
            
            elem = strtok_r(NULL, " ", &next_elem);
            strcpy(status_values[idx], elem);
            name = strtok_r(status_values[idx], ":", &value);
            if (ZCD_STATUS_type[idx] == 0)
            {
                num = strtol(value, NULL, 16);
            }
            else
            {
                num = ZING_get_zcd_status_type(idx, value[0]);
            }
            sprintf(msg, "%s %X\r\n", name, num);
            UART_DBG_UartPutString(msg);
            idx = idx + 1;
        }
        else
        {
            return 1;
        }
    }
    
    return 1;
}


static uint8_t ZING_get_zch_status_type(uint8_t idx, char value)
{
    uint8_t num;
    
    switch (ZCH_STATUS_type[idx])
    {
        case 1:
            if (value == 'L')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 2:
            if (value == 'I')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 3:
            if (value == 'N')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 4:
            if (value == 'P')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 5:
            if (value == 'N')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
    }
    
    return num;
}

static uint8_t ZING_get_zcd_status_type(uint8_t idx, char value)
{
    uint8_t num;
    
    switch (ZCD_STATUS_type[idx])
    {
        case 2:
            if (value == 'I')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 3:
            if (value == 'N')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 4:
            if (value == 'P')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 5:
            if (value == 'N')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
        case 6:
            if (value == 'N')
            {
                num = 0;
            }
            else
            {
                num = 1;
            }
            break;
    }
    
    return num;
}