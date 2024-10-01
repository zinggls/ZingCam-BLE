#ifndef AADC_H
#define AADC_H

#include "project.h"

void AADC_init(void);
int16_t AADC_measure(uint8_t channel);
uint8_t AADC_get_battery_level(int16_t adc_value);

#endif