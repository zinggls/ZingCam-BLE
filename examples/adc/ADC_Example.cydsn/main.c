#include <stdio.h>

#include "project.h"

#define BATTERY_FULL_CHARGE_THRESHOLD 1610
#define BATTERY_NEED_CHARGE_THRESHOLD 1568

#define BATTERY_FULL_CHARGE 0 
#define BATTERY_NEED_CHARGE 1 
#define BATTERY_CRITICAL_LOW 2

static uint32_t ZCBLE_systick = 0;

void ZCBLE_systick_isr(void)
{
    ZCBLE_systick = ZCBLE_systick + 1;
}

uint8_t AADC_get_battery_level(int16_t adc_value)
{
    uint8_t level;
    
    if (adc_value < BATTERY_FULL_CHARGE_THRESHOLD)
    {
        level = BATTERY_FULL_CHARGE;
    }
    else if ((adc_value > BATTERY_NEED_CHARGE_THRESHOLD) && (adc_value <= BATTERY_FULL_CHARGE_THRESHOLD))
    {
        level = BATTERY_NEED_CHARGE;
    }
    else if (adc_value < BATTERY_NEED_CHARGE_THRESHOLD)
    {
        level = BATTERY_CRITICAL_LOW;
    }
    
    return level;
}

int main(void)
{
    uint16_t result;
    char msg[128];
    int cnt;
    uint8_t level;
    
    CySysTickStart();

    for (uint8_t i = 0; i < CY_SYS_SYST_NUM_OF_CALLBACKS; ++i)
    {
        if (CySysTickGetCallback(i) == NULL)
        {
            CySysTickSetCallback(i, ZCBLE_systick_isr);
            break;
        }
    }
    UART_DBG_Start();
    ADC_Start();
    
    cnt = 0;
    
    while (1)
    {
        ADC_StartConvert();
        ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
        result = ADC_GetResult16(0);
        
        sprintf(msg, "%d. res=%d\r\n", cnt++, result);
        //UART_DBG_PutString(msg);
        
        level = AADC_get_battery_level(result);
        
        switch (level)
        {
            case BATTERY_FULL_CHARGE:
                if (SW_USER0_Read() == 0)
                {
                    LED_USER0_Write(0);
                }
                break;
            case BATTERY_NEED_CHARGE:
                if (SW_USER0_Read() == 0)
                {
                    if ((ZCBLE_systick % 1000) == 0)
                    {
                        LED_USER0_Write(!(LED_USER0_Read()));
                    }
                }
                break;
            case BATTERY_CRITICAL_LOW:
                if (SW_USER0_Read() == 0)
                {
                    if ((ZCBLE_systick % 100) == 0)
                    {
                        LED_USER0_Write(!(LED_USER0_Read()));
                    }
                }
                break;
        }
    }
}