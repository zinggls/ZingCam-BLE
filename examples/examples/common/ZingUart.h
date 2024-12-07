#ifndef ZING_UART_H
#define ZING_UART_H

#include <project.h>
#include <UartBuf.h>
    
#define ASCII_LF '\n'
    
typedef void (*ZingRxCallback)(const char *buf);

void ZingUart_Init(ZingRxCallback cb);
CY_ISR(UART_ZING_RX_INTERRUPT);
void zing_process_uart_data();

#endif /* ZING_UART_H */