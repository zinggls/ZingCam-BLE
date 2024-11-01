#include "UartBuf.h"

void UartBuf_init(UartBuf *buf)
{
	buf->write_index = 0;
	buf->read_index = 0;
	buf->message_complete = false;
}

bool UartBuf_is_empty(UartBuf *buf) {
    return buf->write_index == buf->read_index;
}

bool UartBuf_is_full(UartBuf *buf) {
    return ((buf->write_index + 1) % MAX_BUFFER_LENGTH) == buf->read_index;
}

char UartBuf_read_char(UartBuf *buf) {
    char ch = buf->buffer[buf->read_index];
    buf->read_index = (buf->read_index + 1) % MAX_BUFFER_LENGTH;
    return ch;
}

void UartBuf_write_char(UartBuf *buf, char ch) {
    buf->buffer[buf->write_index] = ch;
    buf->write_index = (buf->write_index + 1) % MAX_BUFFER_LENGTH;
    if (buf->write_index == buf->read_index) {  // Buffer overflow, advance read_index
        buf->read_index = (buf->read_index + 1) % MAX_BUFFER_LENGTH;
    }
}