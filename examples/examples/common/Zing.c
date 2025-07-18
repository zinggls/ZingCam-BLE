#include "Zing.h"
    
static UartBuf uBuf;    //Circular buffer for UART data
static ZingRxCallback zingRxCb = NULL;
static uint16 zingRxCbCountPrev,zingRxCbCount;

void Zing_Init(ZingRxCallback cb)
{
    UartBuf_init(&uBuf);
    zingRxCb = cb;
    zingRxCbCountPrev = zingRxCbCount = 0;
}
    
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

// Function to process data when a complete message is available
void zing_process_uart_data()
{
    if (uBuf.message_complete) {
        // Extract complete message from buffer        
        char zing_status[MAX_BUFFER_LENGTH] = {0};
        uint16_t cnt = 0;
        
        while (!UartBuf_is_empty(&uBuf)) {
            zing_status[cnt++] = UartBuf_read_char(&uBuf);
        }
        
        zing_status[cnt] = '\0';  // Null-terminate the string
        uBuf.message_complete = false;
        
        CYASSERT(zingRxCb);
        zingRxCb(zing_status);
        zingRxCbCount++;
    }
}

uint8 getZingState()
{
    if(zingRxCbCount==zingRxCbCountPrev) {
        return 1;
    }else{
        zingRxCbCountPrev = zingRxCbCount;
        return 0;
    }
}

void setZingState(uint8 val, uint8 errCode, uint8 *buf)
{
    if(val==0) {
        *buf = val;
    }else{
        *buf = errCode;
    }
}