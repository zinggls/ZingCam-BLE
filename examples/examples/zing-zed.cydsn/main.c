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

#define ASCII_HOST 'Z'
#define ASCII_DEVICE 'Z'
#define ASCII_LF '\n'
#define MAX_BUFFER_LENGTH 256
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
    int usb;            // USB value, e.g., "2"
    char bnd;           // BND value, e.g., "L"
    unsigned int ppid;  // PPID value, e.g.,"0xABCD"
    unsigned int devid; // DeviceID, e.g.,  "0x3500"
    char trt;           // TRT value, e.g., "B"
    char ack;           // ACK value, e.g., "N"
    char ppc;           // PPC value, e.g., "P"
    unsigned int txid;  // TXID value, e.g.,"0x0"
    unsigned int rxid;  // RXID value, e.g.,"0x0"
    char run;           // RUN value, e.g., "N"
    unsigned int cnt;   // CNT value, e.g., "0"
} ZING_Data;

static char msg[256];

// Circular buffer for UART data
static char uart_buffer[MAX_BUFFER_LENGTH];
static uint16_t write_index = 0;
static uint16_t read_index = 0;
static bool message_complete = false;
ZING_Data zing_data;

// Function to check if buffer is empty
static bool is_buffer_empty() {
    return write_index == read_index;
}

// Function to check if buffer is full
static bool is_buffer_full() {
    return ((write_index + 1) % MAX_BUFFER_LENGTH) == read_index;
}

// Function to read one character from the circular buffer
static char buffer_read_char() {
    char ch = uart_buffer[read_index];
    read_index = (read_index + 1) % MAX_BUFFER_LENGTH;
    return ch;
}

// Function to write one character to the circular buffer
static void buffer_write_char(char ch) {
    uart_buffer[write_index] = ch;
    write_index = (write_index + 1) % MAX_BUFFER_LENGTH;
    if (write_index == read_index) {  // Buffer overflow, advance read_index
        read_index = (read_index + 1) % MAX_BUFFER_LENGTH;
    }
}

CY_ISR(UART_ZING_RX_INTERRUPT)
{
    char ch = UART_ZING_GetChar();
    if (ch != 0) {
        buffer_write_char(ch);  // Write character to circular buffer

        // Check for end of message
        if (ch == ASCII_LF) {
            message_complete = true;
        }
    }

    // Clear the interrupt to prevent retriggering
    UART_ZING_RX_ClearInterrupt();
}

static void PrintZingData(ZING_Data* z) {
    sprintf(msg, "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d\r\n",
        z->usb, z->bnd, z->ppid, z->devid, z->trt, z->ack, z->ppc, z->txid, z->rxid, z->run, z->cnt);
    UART_DBG_UartPutString(msg);
}

// Function to process data when a complete message is available
static void process_uart_data()
{
    if (message_complete) {
        // Extract complete message from buffer
        char zing_status[MAX_BUFFER_LENGTH] = {0};
        uint16_t cnt = 0;
        
        while (!is_buffer_empty()) {
            zing_status[cnt++] = buffer_read_char();
        }
        
        zing_status[cnt] = '\0';  // Null-terminate the string
        message_complete = false;

        // Parsing the values into the structure
        if (sscanf(zing_status, 
                   "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                   &zing_data.usb,
                   &zing_data.bnd,
                   &zing_data.ppid,
                   &zing_data.devid,
                   &zing_data.trt,
                   &zing_data.ack,
                   &zing_data.ppc,
                   &zing_data.txid,
                   &zing_data.rxid,
                   &zing_data.run,
                   &zing_data.cnt) != 11) {
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
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        process_uart_data();
    }
}

/* [] END OF FILE */
