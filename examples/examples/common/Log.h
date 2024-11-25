#ifndef LOG_H
#define LOG_H

#include <project.h>
#include <stdarg.h>
#include <stdio.h>

#define LOG_BUFFER_SIZE 1024

static char buffer[LOG_BUFFER_SIZE];

void L(const char *format, ...)
{
    va_list args;

    // Initialize the variable argument list
    va_start(args, format);

    // Format the string
    vsnprintf(buffer, LOG_BUFFER_SIZE, format, args);

    // End using variable argument list
    va_end(args);

    // Send the formatted string over UART
    UART_DBG_UartPutString(buffer);
}

#endif /* LOG_H */