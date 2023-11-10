#include <stdio.h>
#include <string.h>

#include "project.h"

#define ASCII_HOST 'H'
#define ASCII_DEVICE 'D'
#define ASCII_LF '\n'
#define NUM_DEVICE_STATUS 12

void ZING_init(uint8_t** zing_device_status_values);
uint8_t ZING_get_device_status(char* data);
void ZING_parse_device_status(char* device_status, uint8_t** values);
