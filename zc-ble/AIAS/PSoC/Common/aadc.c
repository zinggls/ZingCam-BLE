#include "aadc.h"
#include "icd.h"

#define BATTERY_FULL_CHARGE 1610
#define BATTERY_NEED_CHARGE 1568

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

uint8_t AADC_get_battery_level(int16_t adc_value)
{
    uint8_t level;
    
    if (adc_value < BATTERY_FULL_CHARGE)
    {
        level = 0;
    }
    else if ((adc_value > BATTERY_NEED_CHARGE) && (adc_value <= BATTERY_FULL_CHARGE))
    {
        level = 1;
    }
    else if (adc_value < BATTERY_NEED_CHARGE)
    {
        level = 2;
    }
    
    return level;
}