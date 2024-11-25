#ifndef ZING_UART_H
#define ZING_UART_H

#include <project.h>
#include <UartBuf.h>
    
#define ASCII_LF '\n'
    
static UartBuf uBuf;    //Circular buffer for UART data
    
CY_ISR(UART_ZING_RX_INTERRUPT)
{
    char ch = UART_ZING_GetChar();
    if (ch != 0) {
        UartBuf_write_char(&uBuf,ch);  // Write character to circular buffer

        // Check for end of message
        if (ch == ASCII_LF) {
            uBuf.message_complete = true;
        }
    }

    // Clear the interrupt to prevent retriggering
    UART_ZING_RX_ClearInterrupt();
}

#endif /* ZING_UART_H */