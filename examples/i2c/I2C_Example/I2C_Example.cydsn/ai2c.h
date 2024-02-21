#ifndef AI2C_H
#define AI2C_H

#include "project.h"    
    
void AI2C_init(void);
#if I2C_I2C_MASTER
uint8_t AI2C_write(uint8_t addr, uint8_t* data, uint8_t len);
uint8_t AI2C_read(uint8_t addr, uint8_t* data, uint8_t len);
#endif
#if I2C_I2C_SLAVE
uint8_t AI2C_write(uint8_t* data, uint8_t len);
uint8_t AI2C_read(uint8_t* data, uint8_t len);
#endif
    
#endif