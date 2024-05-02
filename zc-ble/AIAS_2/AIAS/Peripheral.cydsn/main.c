#include "../common/headers/main.h"

static uint32_t AIAS_systick;

static void AIAS_systick_isr(void);

int main(void)
{
    CyGlobalIntEnable;
    CySysTickStart();
    
    char* zing_status;
    char** status_values;
         
    ZING_init();
    Debug_init();
    
    zing_status = (char*)malloc(sizeof(char) * MAX_ZING_STATUS_LENGTH);
    status_values = (char**)malloc(sizeof(char*) * NUM_ZCD_STATUS);
    for (uint8_t i = 0; i < NUM_ZCD_STATUS; i++)
    {
        status_values[i] = (char*)malloc(sizeof(char) * MAX_STATUS_VALUE_LENGTH);
    }
    AIAS_systick = 0;
    
    for (uint8_t i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            CySysTickSetCallback(i, AIAS_systick_isr);
            break;
        }
    }
        
    while (1)
    {
        ZING_get_status(zing_status);
        ZING_parse_zcd_status(zing_status, status_values);
        
        UART_DBG_UartPutString("Hello World\n");
    }
}

static void AIAS_systick_isr(void)
{
    AIAS_systick = AIAS_systick + 1;
}

int AIAS_get_systick(void)
{
    return AIAS_systick;
}