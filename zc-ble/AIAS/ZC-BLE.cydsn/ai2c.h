#ifndef AI2C_H
#define AI2C_H

#include "project.h"    
#include "main.h"    
    
void AI2C_init(void);
#if HBLE
uint8_t AI2C_write(uint8_t addr, uint8_t* data, uint8_t len);
uint8_t AI2C_read(uint8_t addr, uint8_t* data, uint8_t len);
#endif
#if DBLE
uint8_t AI2C_write(uint8_t* data, uint8_t len);
uint8_t AI2C_read(uint8_t* data, uint8_t len);
#endif
    
#endif