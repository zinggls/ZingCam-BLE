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
    DEVICE_STATUS_USB,
    DEVICE_STATUS_PPID,
    DEVICE_STATUS_DEVID,
    DEVICE_STATUS_FMT,
    DEVICE_STATUS_IDX,
    DEVICE_STATUS_FPS,
    DEVICE_STATUS_TRT,
    DEVICE_STATUS_ACK,
    DEVICE_STATUS_PPC,
    DEVICE_STATUS_RUN,
    DEVICE_STATUS_ITF,
    DEVICE_STATUS_TXID,
    DEVICE_STATUS_RXID,
    DEVICE_STATUS_DST_ID_ERR_CNT,
    DEVICE_STATUS_PHY_RX_FRAME_CNT,
    DEVICE_STATUS_MFIR,
    DEVICE_STATUS_CNT,
    NUM_DEVICE_STATUS
} ZING_DEVICE_STATUS_NAME;

typedef struct {
    int usb;                // USB value, e.g., "2"
    unsigned int ppid;      // PPID value, e.g., "0xABCD"
    unsigned int devid;     // DeviceID, e.g., "0x3500"
    int fmt;                // FMT value, e.g., "3"
    int idx;                // IDX value, e.g., "3"
    unsigned int fps;       // FPS value, e.g., "0x51615"
    char trt;               // TRT value, e.g., "B"
    char ack;               // ACK value, e.g., "N"
    char ppc;               // PPC value, e.g., "D"
    char run;               // RUN value, e.g., "Y"
    char itf;               // ITF value, e.g., "N"
    unsigned int txid;      // TXID value, e.g., "0x0"
    unsigned int rxid;      // RXID value, e.g., "0x35ABCD"
    int dest_err_cnt;       // DestID_ERR_CNT value before parentheses, e.g., "3"
    int dest_err_sub;       // DestID_ERR_CNT value inside parentheses, e.g., "0"
    int phy_rx_frame_cnt;   // PHY_RX_FRAME_CNT before parentheses, e.g., "1534345"
    int phy_rx_sub;         // PHY_RX_FRAME_CNT inside parentheses, e.g., "232"
    int mfir_main;          // MFIR value before '/', e.g., "0"
    int mfir_sub;           // MFIR value after '/', e.g., "232"
    unsigned int cnt;       // CNT value, e.g., "30899"
} ZING_Data;

static char msg[256];
static UartBuf uBuf;    //Circular buffer for UART data
static ZING_Data zing_data;

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
    sprintf(msg, "ZCD USB:%d PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d FPS:0x%X TRT:%c ACK:%c PPC:%c RUN:%c ITF:%c TXID:0x%X RXID:0x%X DestID_ERR_CNT:%d(%d) PHY_RX_FRAME_CNT:%d(%d) MFIR:%d/%d CNT:%u\r\n",
        z->usb, z->ppid, z->devid, z->fmt, z->idx, z->fps, z->trt, z->ack, z->ppc, z->run, z->itf, z->txid, z->rxid, z->dest_err_cnt, z->dest_err_sub, z->phy_rx_frame_cnt, z->phy_rx_sub, z->mfir_main, z->mfir_sub, z->cnt);
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

        // Parse the message
        if (sscanf(zing_status, 
                   "ZCD USB:%d PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d FPS:0x%X TRT:%c ACK:%c PPC:%c RUN:%c ITF:%c TXID:0x%X RXID:0x%X DestID_ERR_CNT:%d(%d) PHY_RX_FRAME_CNT:%d(%d) MFIR:%d/%d CNT:%u",
                   &zing_data.usb,
                   &zing_data.ppid,
                   &zing_data.devid,
                   &zing_data.fmt,
                   &zing_data.idx,
                   &zing_data.fps,
                   &zing_data.trt,
                   &zing_data.ack,
                   &zing_data.ppc,
                   &zing_data.run,
                   &zing_data.itf,
                   &zing_data.txid,
                   &zing_data.rxid,
                   &zing_data.dest_err_cnt,
                   &zing_data.dest_err_sub,
                   &zing_data.phy_rx_frame_cnt,
                   &zing_data.phy_rx_sub,
                   &zing_data.mfir_main,
                   &zing_data.mfir_sub,
                   &zing_data.cnt) != 20) {
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
