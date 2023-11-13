#ifndef ZING_H
#define ZING_H
    
#include <stdio.h>
#include <string.h>

#include "project.h"

#define ASCII_HOST 'H'
#define ASCII_DEVICE 'D'
#define ASCII_LF '\n'
#define MAX_BUFFER_LENGTH 128
#define NUM_HOST_STATUS 14
#define NUM_DEVICE_STATUS 12
#define MAX_VALUE_LENGTH 12

uint8_t** ZING_host_init(void);
uint8_t** ZING_device_init(void);
uint8_t ZING_get_host_status(char* data);
uint8_t ZING_get_device_status(char* data);
uint8_t ZING_parse_host_status(char* device_status, uint8_t** status_values);
uint8_t ZING_parse_device_status(char* device_status, uint8_t** status_values);

#endif