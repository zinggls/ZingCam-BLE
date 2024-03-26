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
uint8_t AI2C_write(void);
uint8_t AI2C_read(void);
void AI2C_set_read_buffer(uint8_t* data, uint8_t len);
void AI2C_set_write_buffer(uint8_t* data, uint8_t len);
#endif
    
#endif