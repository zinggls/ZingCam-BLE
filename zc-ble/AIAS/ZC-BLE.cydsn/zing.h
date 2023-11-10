#include <stdio.h>
#include <string.h>

#include "project.h"

#define HOST_COMMAND_LENGTH 4
#define DEVICE_COMMAND_LENGTH 6

void ZING_init(void);

uint8_t ZING_get_device_status(char* data);
