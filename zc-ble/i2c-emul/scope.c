#include <stdio.h>
#include <stdint.h>

typedef struct {
	uint8_t camera;
	uint8_t output;
	uint8_t mode;
	uint8_t battery_status;
	uint8_t ir_status;
	uint8_t eo_status;
} Scope;

int main(int argc, char *argv[])
{  
	printf("I2C master emulating the scope\r\n");
}  
