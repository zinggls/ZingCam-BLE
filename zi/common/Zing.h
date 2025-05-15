#ifndef ZING_H
#define ZING_H

#include <project.h>
#include <UartBuf.h>
    
#define ASCII_LF '\n'
    
typedef void (*ZingRxCallback)(const char *buf);

void Zing_Init(ZingRxCallback cb);
CY_ISR(UART_ZING_RX_INTERRUPT);
void zing_process_uart_data();
uint8 getZingState();
void setZingState(uint8 val, uint8 errCode, uint8 *buf);
void setTxZingState(uint8 val, char run, uint8 errCode, uint8 *buf);
void setCh(uint8 ch);
void UartRestart(ZingRxCallback cb);

#endif /* ZING_H */