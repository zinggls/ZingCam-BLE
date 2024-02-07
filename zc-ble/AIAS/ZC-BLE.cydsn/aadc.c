#include "aadc.h"

#define BATTERY_THRESHOLD 1475

void AADC_init(void)
{
    ADC_Start();
}

int16_t AADC_measure(uint8_t channel)
{
    int16_t result;
    
    ADC_StartConvert();
    ADC_IsEndConversion(ADC_WAIT_FOR_RESULT);
    
    result = ADC_GetResult16(channel);
    
    return result;
}

uint8_t AADC_get_battery_percent(int16_t adc_value)
{
    uint8_t percent;
    
    if (adc_value < BATTERY_THRESHOLD)
    {
        percent = 0;
    }
    else
    {
        percent = 1; // 1 ~ 100
    }
    
    return percent;
}