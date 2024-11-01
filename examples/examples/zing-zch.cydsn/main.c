/* ========================================
 *
 * Copyright YOUR COMPANY, THE YEAR
 * All Rights Reserved
 * UNPUBLISHED, LICENSED SOFTWARE.
 *
 * CONFIDENTIAL AND PROPRIETARY INFORMATION
 * WHICH IS THE PROPERTY OF your company.
 *
 * ========================================
*/
#include "project.h"
#include <stdio.h>
#include <stdbool.h>
#include <UartBuf.h>

#define ASCII_HOST 'Z'
#define ASCII_DEVICE 'Z'
#define ASCII_LF '\n'
#define MAX_VALUE_LENGTH 32

typedef enum { 
    HOST_STATUS_USB,
    HOST_STATUS_VND,
    HOST_STATUS_PRD,
    HOST_STATUS_BND,
    HOST_STATUS_PPID,
    HOST_STATUS_DEVID,
    HOST_STATUS_FMT,
    HOST_STATUS_IDX,
    HOST_STATUS_TRT,
    HOST_STATUS_ACK,
    HOST_STATUS_PPC,
    HOST_STATUS_TXID,
    HOST_STATUS_RXID,
    HOST_STATUS_RUN,
    HOST_STATUS_CNT,
    NUM_HOST_STATUS
} ZING_HOST_STATUS_NAME;

typedef struct {
    int usb;             // USB field value (e.g., 2)
    int vendor_id;       // VND field value (e.g., 0xEB1A)
    int product_id;      // PRD field value (e.g., 0xB101)
    char band;           // BND field value (e.g., 'L')
    int ppid;            // PPID field value (e.g., 0xABCD)
    int device_id;       // DeviceID field value (e.g., 0x3500)
    int format;          // FMT field value (e.g., 1)
    int index;           // IDX field value (e.g., 2)
    char trt;            // TRT field value (e.g., 'B')
    char ack;            // ACK field value (e.g., 'N')
    char ppc;            // PPC field value (e.g., 'P')
    int txid;            // TXID field value (e.g., 0x35ABCD)
    int rxid;            // RXID field value (e.g., 0x0)
    char run;            // RUN field value (e.g., 'Y')
    unsigned int cnt;    // CNT field value (e.g., 36119)
} ZING_Data;


static char msg[256];

// Circular buffer for UART data
UartBuf uBuf;
ZING_Data zing_data;

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

static void PrintZingData(ZING_Data* z) {
    sprintf(msg,"ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d\r\n",
        z->usb, z->vendor_id, z->product_id, z->band, z->ppid, z->device_id, z->format, z->index, z->trt, z->ack, z->ppc, z->txid, z->rxid, z->run, z->cnt);
    UART_DBG_UartPutString(msg);
}

// Function to process data when a complete message is available
static void process_uart_data()
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

        // Parsing the values into the structure
        if (sscanf(zing_status, 
                   "ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                   &zing_data.usb,
                   &zing_data.vendor_id,
                   &zing_data.product_id,
                   &zing_data.band,
                   &zing_data.ppid,
                   &zing_data.device_id,
                   &zing_data.format,
                   &zing_data.index,
                   &zing_data.trt,
                   &zing_data.ack,
                   &zing_data.ppc,
                   &zing_data.txid,
                   &zing_data.rxid,
                   &zing_data.run,
                   &zing_data.cnt) != 15) {
            UART_DBG_UartPutString("Parsing Error\r\n");
            UART_DBG_UartPutString("Received: ");
            UART_DBG_UartPutString(zing_status);
            UART_DBG_UartPutString("\r\n");
        } else {
            PrintZingData(&zing_data);
        }
    }
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_DBG_Start();
    UART_ZING_Start();
    UartBuf_init(&uBuf);
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        process_uart_data();
    }
}

/* [] END OF FILE */
