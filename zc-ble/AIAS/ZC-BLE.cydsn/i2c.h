#ifndef I2C_H
#define I2C_H

#include "project.h"    
    
void I2C_init(void);
uint8_t I2C_write(uint8_t addr, uint8_t* data, uint8_t len);
    
#endif