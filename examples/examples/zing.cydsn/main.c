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
    char usb[4];        // USB value, e.g., "2"
    char bnd[2];        // BND value, e.g., "L"
    char ppid[8];       // PPID value, e.g., "0xABCD"
    char devid[8];      // DeviceID, e.g., "0x3500"
    char trt[2];        // TRT value, e.g., "B"
    char ack[2];        // ACK value, e.g., "N"
    char ppc[2];        // PPC value, e.g., "P"
    char txid[8];       // TXID value, e.g., "0x0"
    char rxid[8];       // RXID value, e.g., "0x0"
    char run[2];        // RUN value, e.g., "N"
    char cnt[4];        // CNT value, e.g., "0"
} ZING_Data;

static uint16_t uart_loop = 0;
static char zing_status[MAX_BUFFER_LENGTH];
static uint8_t ZED = 0;

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

            // Parse the zing_status buffer
            UART_DBG_UartPutString("Received: ");
            UART_DBG_UartPutString(zing_status);
            UART_DBG_UartPutString("\r\n");

            // Example of parsing the values into the structure
            sscanf(zing_status, "ZED USB:%3s BND:%1s PPID:%7s DeviceID:%7s TRT:%1s ACK:%1s PPC:%1s TXID:%7s RXID:%7s RUN:%1s CNT:%3s",
                zing_data.usb,
                zing_data.bnd,
                zing_data.ppid,
                zing_data.devid,
                zing_data.trt,
                zing_data.ack,
                zing_data.ppc,
                zing_data.txid,
                zing_data.rxid,
                zing_data.run,
                zing_data.cnt
            );

            // Debug output to verify parsing
            UART_DBG_UartPutString("Parsed values:\r\n");
            UART_DBG_UartPutString("USB: "); UART_DBG_UartPutString(zing_data.usb); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("BND: "); UART_DBG_UartPutString(zing_data.bnd); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("PPID: "); UART_DBG_UartPutString(zing_data.ppid); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("DeviceID: "); UART_DBG_UartPutString(zing_data.devid); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("TRT: "); UART_DBG_UartPutString(zing_data.trt); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("ACK: "); UART_DBG_UartPutString(zing_data.ack); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("PPC: "); UART_DBG_UartPutString(zing_data.ppc); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("TXID: "); UART_DBG_UartPutString(zing_data.txid); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("RXID: "); UART_DBG_UartPutString(zing_data.rxid); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("RUN: "); UART_DBG_UartPutString(zing_data.run); UART_DBG_UartPutString("\r\n");
            UART_DBG_UartPutString("CNT: "); UART_DBG_UartPutString(zing_data.cnt); UART_DBG_UartPutString("\r\n");

            // Reset buffer and counter for next message
            memset(zing_status, 0, sizeof(zing_status));
            cnt = 0;
        }
    }

    // Clear the interrupt to prevent retriggering
    UART_ZING_RX_ClearInterrupt();
}

uint8_t ZING_parse_host_status(uint8_t** status_values)
{
    char* status;
    char* next_ptr;
    uint8_t cnt;
    uint8_t idx;
    
    cnt = 0;
    idx = 0;
    
    status = strtok_r(zing_status, " :", &next_ptr);
    
    if (status == NULL)
    {
        return 0;
    }
    
    if (strcmp(status, "ZED") == 0)
    {
        ZED = 1;
    }
    else
    {
        ZED = 0;
    }
    
    while ((status = strtok_r(NULL, " :", &next_ptr)) != NULL)
    {
        if ((cnt % 2) == 1)
        {
            if (idx < NUM_HOST_STATUS)
            {
                if (strlen(status) < MAX_VALUE_LENGTH)
                {                   
                    memset(status_values[idx], 0, MAX_VALUE_LENGTH);
                    memcpy(status_values[idx++], status, strlen(status));
                }
                else
                {
                    return 0;
                }
            }
            else
            {
                return 0;
            }
        }
        
        cnt = cnt + 1;
    }
    return 1;
}

int main(void)
{
    uint8_t** zing_host_status_values = 0;
    CyGlobalIntEnable; /* Enable global interrupts. */

    /* Place your initialization/startup code here (e.g. MyInst_Start()) */
    UART_DBG_Start();
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);

    UART_DBG_UartPutString("Start\r\n");
    for(;;)
    {
        /* Place your application code here. */
        ZING_parse_host_status(zing_host_status_values);
    }
}

/* [] END OF FILE */
