#ifndef UART_BUF_H
#define UART_BUF_H

#include <stdint.h>
#include <stdbool.h>

#define MAX_BUFFER_LENGTH 1024

typedef struct {
    char buffer[MAX_BUFFER_LENGTH];
    uint16_t write_index;
    uint16_t read_index;
    bool message_complete;
} UartBuf;

void UartBuf_init(UartBuf *buf);
bool UartBuf_is_empty(UartBuf *buf);
bool UartBuf_is_full(UartBuf *buf);
char UartBuf_read_char(UartBuf *buf);
void UartBuf_write_char(UartBuf *buf, char ch);

#endif /* UART_BUF_H */