#ifndef LED_H
#define LED_H
    
#include <project.h>

/* LED constants */
#define LED_ON              (0u)
#define LED_OFF             (1u)

void setRed(uint8 red);
void setGreen(uint8 green);
void setBlue(uint8 blue);
void setRGB(uint8 red, uint8 green, uint8 blue);
    
#endif //LED_H