#ifndef ZING_H
#define ZING_H

#include <project.h>
#include <UartBuf.h>
    
#define ASCII_LF '\n'
    
typedef void (*ZingRxCallback)(const char *buf);

void Zing_Init(ZingRxCallback cb);
CY_ISR(UART_ZING_RX_INTERRUPT);
void zing_process_uart_data();

#endif /* ZING_H */