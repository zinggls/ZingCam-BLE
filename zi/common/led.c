#include "led.h"
#include <project.h>

void setRed(uint8 red)
{
    LED_RED_Write(red);
    LED_GREEN_Write(LED_OFF);
    LED_BLUE_Write(LED_OFF);
}

void setGreen(uint8 green)
{
    LED_RED_Write(LED_OFF);
    LED_GREEN_Write(green);
    LED_BLUE_Write(LED_OFF);
}

void setBlue(uint8 blue)
{
    LED_RED_Write(LED_OFF);
    LED_GREEN_Write(LED_OFF);
    LED_BLUE_Write(blue);
}

void setRGB(uint8 red, uint8 green, uint8 blue)
{
    LED_RED_Write(red);
    LED_GREEN_Write(green);
    LED_BLUE_Write(blue);
}