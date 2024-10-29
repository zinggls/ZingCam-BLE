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

// Define the type for the callback function
typedef void (*EventCallback)(ZING_Data*);

static EventCallback event_callback = NULL;
static char msg[256];

// Function to register a callback
void RegisterEventCallback(EventCallback callback) {
    event_callback = callback;
}

void OnDataReceived(ZING_Data* z) {
    sprintf(msg, "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d\r\n",
        z->usb, z->bnd, z->ppid, z->devid, z->trt, z->ack, z->ppc, z->txid, z->rxid, z->run, z->cnt);
    UART_DBG_UartPutString(msg);
}

CY_ISR(UART_ZING_RX_INTERRUPT)
{
    static uint16_t cnt = 0;
    static char zing_status[MAX_BUFFER_LENGTH] = {0};  // Buffer to store the received data
    ZING_Data zing_data;  // Structure to store parsed values

    char ch = UART_ZING_GetChar();
    if (ch != 0) {
        // Add received character to zing_status buffer
        if (cnt < MAX_BUFFER_LENGTH - 1) {
            zing_status[cnt++] = ch;
        }

        // Check if we've received the full string (terminated by newline)
        if (ch == ASCII_LF) {
            zing_status[cnt] = '\0';  // Null-terminate the string

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
            } else {
                if (event_callback != NULL) event_callback(&zing_data);
            }

            // Reset buffer and counter for next message
            memset(zing_status, 0, sizeof(zing_status));
            cnt = 0;
        }
    }

    // Clear the interrupt to prevent retriggering
    UART_ZING_RX_ClearInterrupt();
}

int main(void)
{
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_DBG_Start();
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);

    // Register the event callback
    RegisterEventCallback(OnDataReceived);
    
    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
    }
}

/* [] END OF FILE */
