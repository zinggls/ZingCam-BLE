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

// Function to initialize UartBuf
void UartBuf_init(UartBuf *buf)
{
	buf->write_index = 0;
	buf->read_index = 0;
	buf->message_complete = false;
}

// Function to check if UartBuf is empty
bool UartBuf_is_empty(UartBuf *buf) {
    return buf->write_index == buf->read_index;
}

// Function to check if UartBuf is full
bool UartBuf_is_full(UartBuf *buf) {
    return ((buf->write_index + 1) % MAX_BUFFER_LENGTH) == buf->read_index;
}

// Function to read one character from the UartBuf
char UartBuf_read_char(UartBuf *buf) {
    char ch = buf->buffer[buf->read_index];
    buf->read_index = (buf->read_index + 1) % MAX_BUFFER_LENGTH;
    return ch;
}

// Function to write one character to the UartBuf
void UartBuf_write_char(UartBuf *buf, char ch) {
    buf->buffer[buf->write_index] = ch;
    buf->write_index = (buf->write_index + 1) % MAX_BUFFER_LENGTH;
    if (buf->write_index == buf->read_index) {  // Buffer overflow, advance read_index
        buf->read_index = (buf->read_index + 1) % MAX_BUFFER_LENGTH;
    }
}

#endif /* UART_BUF_H */