#ifndef AADC_H
#define AADC_H

#include "project.h"

void AADC_init(void);
int16_t AADC_measure(uint8_t channel);

#endif