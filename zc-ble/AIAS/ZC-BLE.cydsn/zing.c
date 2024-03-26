#include <stdlib.h>

#include "zing.h"
#include "icd.h"

#define NUM_ZING_CNT 10

static uint8_t state = 0;
static uint32_t ZING_parse_systick;
static uint16_t cnt_tmp = 0;
static uint16_t uart_loop = 0;
static uint8_t current_channel = 0;

static char zing_status[MAX_BUFFER_LENGTH];

static uint16_t ZING_get_device_status_dst_id_err_diff_cnt(char* dst_id_err_cnt_end_ptr);
static uint16_t ZING_get_device_status_phy_rx_frame_diff_cnt(char* phy_rx_frame_cnt_end_ptr);
static uint16_t ZING_get_device_status_mfir_phy_rx_frame_diff(char* dst_id_err_diff_end_ptr);

uint8_t** ZING_host_init(void)
{
    uint8_t** status_values;
    
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    ZING_parse_systick = 0;
    
    status_values = (uint8_t**)calloc(NUM_HOST_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_HOST_STATUS; i++)
    {
        status_values[i] = (uint8_t*)calloc(MAX_VALUE_LENGTH, sizeof(uint8_t));
    }
    
    return status_values;
}

uint8_t** ZING_device_init(void)
{
    uint8_t** status_values;
    
    UART_ZING_Start();
    UART_ZING_RX_INTR_StartEx(UART_ZING_RX_INTERRUPT);
    ZING_parse_systick = 0;
        
    status_values = (uint8_t**)calloc(NUM_DEVICE_STATUS, sizeof(uint8_t*));
    for (uint8_t i = 0; i < NUM_DEVICE_STATUS; i++)
    {
        status_values[i] = (uint8_t*)calloc(MAX_VALUE_LENGTH, sizeof(uint8_t));
    }
    
    return status_values;
}

CY_ISR(UART_ZING_RX_INTERRUPT)
{
    char ch;
    uint16_t cnt;
    
    ch = UART_ZING_GetChar();
    cnt = 0;
    uart_loop = uart_loop + 1;
    
    if (ch == ASCII_HOST || ch == ASCII_DEVICE)
    {
        zing_status[cnt++] = ch;
        
        do
        {
            if ((ch = UART_ZING_GetChar()) != 0)
            {
                if (cnt < MAX_BUFFER_LENGTH)
                {
                    zing_status[cnt++] = ch;
                }
            }
        }
        while (ch != ASCII_LF);
    }
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
    
    if (cnt_tmp != uart_loop)
    {
        ZING_parse_systick = ZCBLE_get_systick();
    }
    else
    {
        if (ZCBLE_get_systick() - ZING_parse_systick > 100)
        {
            return 0;
        }
    }
    
    cnt_tmp = uart_loop;
    
    return 1;
}

uint8_t ZING_parse_device_status(uint8_t** status_values)
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
    
    while ((status = strtok_r(NULL, " :", &next_ptr)) != NULL)
    {
        if ((cnt % 2) == 1)
        {
            if (idx < NUM_DEVICE_STATUS)
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
    
    if (cnt_tmp != uart_loop)
    {
        ZING_parse_systick = ZCBLE_get_systick();
    }
    else
    {
        if (ZCBLE_get_systick() - ZING_parse_systick > 100)
        {
            return 0;
        }
    }
    
    cnt_tmp = uart_loop;
    
    return 1;
}

void ZING_change_channel(uint8_t** host_status, uint8_t val)
{
    uint8_t arr[4] = { 0x4, 'b', 0x0, 0x0 };
    
    if (host_status == NULL)
    {
        // val = 0: state no change
        // val = 1: state need change
        state = val;
    }
    else
    {
        if (state == 1)
        {
            if (strcmp((char*)host_status[HOST_STATUS_BND], "L") == 0)
            {   
                arr[2] = 0x1;
                current_channel = 1;
                UART_ZING_PutArray(arr, 4);
            }
            else if (strcmp((char*)host_status[HOST_STATUS_BND], "H") == 0)
            {
                arr[2] = 0x0;
                current_channel = 0;
                UART_ZING_PutArray(arr, 4);
            }
            
            state = 0;
        }
    }
}

void ZING_set_channel_high(void)
{
    uint8_t arr[4] = { 0x4, 'b', 0x0, 0x0 };
    
    if (current_channel != 0)
    {
        current_channel = 0;
        UART_ZING_PutArray(arr, 4);
    }
}

void ZING_set_channel_low(void)
{
    uint8_t arr[4] = { 0x4, 'b', 0x1, 0x0 };
    
    if (current_channel != 1)
    {
        current_channel = 1;
        UART_ZING_PutArray(arr, 4);
    }
}

uint8_t ZING_get_mode(void)
{
    uint8_t mode;
    
    switch (state)
    {
        case 0:
            mode = 2;
        break;
        case 1:
            mode = 1;
        break;
    }
    return mode;
}

uint8_t ZING_get_info(void)
{
    return current_channel;
}

void ZING_reset(void)
{
    uint8_t arr[4] = { 0x4, 'r', 's', 't' };
    
    UART_ZING_PutArray(arr, 4);
}

uint8_t ZING_get_host_status_usb(uint8_t** host_status)
{
    uint8_t usb = atoi((char*)host_status[HOST_STATUS_USB]);
    
    return usb;
}

uint16_t ZING_get_host_status_vnd(uint8_t** host_status)
{
    uint16_t vnd = strtol((char*)host_status[HOST_STATUS_VND], NULL, 16);
    
    return vnd;
}

uint16_t ZING_get_host_status_prd(uint8_t** host_status)
{
    uint16_t prd = strtol((char*)host_status[HOST_STATUS_PRD], NULL, 16);
    
    return prd;
}

char ZING_get_host_status_bnd(uint8_t** host_status)
{
    char bnd = *host_status[HOST_STATUS_BND];
    
    return bnd;
}

uint16_t ZING_get_host_status_ppid(uint8_t** host_status)
{
    uint16_t ppid = strtol((char*)host_status[HOST_STATUS_PPID], NULL, 16);
    
    return ppid;
}

uint16_t ZING_get_host_status_devid(uint8_t** host_status)
{
    uint16_t devid = strtol((char*)host_status[HOST_STATUS_DEVID], NULL, 16);
    
    return devid;
}

uint8_t ZING_get_host_status_fmt(uint8_t** host_status)
{
    uint8_t fmt = atoi((char*)host_status[HOST_STATUS_FMT]);
    
    return fmt;
}

uint8_t ZING_get_host_status_idx(uint8_t** host_status)
{
    uint8_t idx = atoi((char*)host_status[HOST_STATUS_IDX]);
    
    return idx;
}

char ZING_get_host_status_trt(uint8_t** host_status)
{
    char trt = *host_status[HOST_STATUS_TRT];
    
    return trt;
}

char ZING_get_host_status_ack(uint8_t** host_status)
{
    char ack = *host_status[HOST_STATUS_ACK];
    
    return ack;
}

char ZING_get_host_status_ppc(uint8_t** host_status)
{
    char ppc = *host_status[HOST_STATUS_PPC];
    
    return ppc;
}

char ZING_get_host_status_run(uint8_t** host_status)
{
    char run = *host_status[HOST_STATUS_RUN];
    
    return run;
}

uint32_t ZING_get_host_status_txid(uint8_t** host_status)
{
    uint32_t txid = strtol((char*)host_status[HOST_STATUS_TXID], NULL, 16);
    
    return txid;
}

uint32_t ZING_get_host_status_rxid(uint8_t** host_status)
{
    uint32_t rxid = strtol((char*)host_status[HOST_STATUS_RXID], NULL, 16);
    
    return rxid;
}

uint32_t ZING_get_host_status_cnt(uint8_t** host_status)
{
    uint32_t cnt = atoi((char*)host_status[HOST_STATUS_CNT]);
    
    return cnt;
}

uint8_t ZING_get_device_status_usb(uint8_t** device_status)
{
    uint8_t usb = atoi((char*)device_status[DEVICE_STATUS_USB]);
    
    return usb;
}

uint16_t ZING_get_device_status_ppid(uint8_t** device_status)
{
    uint16_t ppid = strtol((char*)device_status[DEVICE_STATUS_PPID], NULL, 16);
    
    return ppid;
}

uint16_t ZING_get_device_status_devid(uint8_t** device_status)
{
    uint16_t devid = strtol((char*)device_status[DEVICE_STATUS_DEVID], NULL, 16);
    
    return devid;
}

uint8_t ZING_get_device_status_fmt(uint8_t** device_status)
{
    uint8_t fmt = atoi((char*)device_status[DEVICE_STATUS_FMT]);
    
    return fmt;
}

uint8_t ZING_get_device_status_idx(uint8_t** device_status)
{
    uint8_t idx = atoi((char*)device_status[DEVICE_STATUS_IDX]);
    
    return idx;
}

char ZING_get_device_status_trt(uint8_t** device_status)
{
    char trt = *device_status[DEVICE_STATUS_TRT];
    
    return trt;
}

char ZING_get_device_status_ack(uint8_t** device_status)
{
    char ack = *device_status[DEVICE_STATUS_ACK];
    
    return ack;
}

char ZING_get_device_status_ppc(uint8_t** device_status)
{
    char ppc = *device_status[DEVICE_STATUS_PPC];
    
    return ppc;
}


char ZING_get_device_status_run(uint8_t** device_status)
{
    char run = *device_status[DEVICE_STATUS_RUN];
    
    return run;
}

char ZING_get_device_status_itf(uint8_t** device_status)
{
    char run = *device_status[DEVICE_STATUS_ITF];
    
    return run;
}

uint32_t ZING_get_device_status_txid(uint8_t** device_status)
{
    uint32_t txid = strtol((char*)device_status[DEVICE_STATUS_TXID], NULL, 16);
    
    return txid;
}

uint32_t ZING_get_device_status_rxid(uint8_t** device_status)
{
    uint32_t rxid = strtol((char*)device_status[DEVICE_STATUS_RXID], NULL, 16);
    
    return rxid;
}

ZING_diff ZING_get_device_status_dst_id_err_cnt(uint8_t** device_status)
{
    ZING_diff dst_id_err;
    char* endptr;
    dst_id_err.cnt = strtol((char*)device_status[DEVICE_STATUS_DST_ID_ERR_CNT], &endptr, 10);
    dst_id_err.diff = ZING_get_device_status_dst_id_err_diff_cnt(endptr);
    
    return dst_id_err;
}

uint16_t ZING_get_device_status_dst_id_err_diff_cnt(char* dst_id_err_cnt_end_ptr)
{
    uint16_t dst_id_err_diff_cnt = strtol(dst_id_err_cnt_end_ptr + 1, NULL, 10);
    
    return dst_id_err_diff_cnt;
}

ZING_diff ZING_get_device_status_phy_rx_frame_cnt(uint8_t** device_status)
{
    ZING_diff phy_rx_frame;
    char* endptr;
    phy_rx_frame.cnt = strtol((char*)device_status[DEVICE_STATUS_PHY_RX_FRAME_CNT], &endptr, 10);
    phy_rx_frame.diff = ZING_get_device_status_phy_rx_frame_diff_cnt(endptr);
    
    return phy_rx_frame;
}

uint16_t ZING_get_device_status_phy_rx_frame_diff_cnt(char* phy_rx_frame_cnt_end_ptr)
{
    uint16_t phy_rx_frame_diff_cnt = strtol(phy_rx_frame_cnt_end_ptr + 1, NULL, 10);
    
    return phy_rx_frame_diff_cnt;
}

ZING_mfir ZING_get_device_status_mfir(uint8_t** device_status)
{
    ZING_mfir mfir;
    char* endptr;
    mfir.dst_id_err_diff = strtol((char*)device_status[DEVICE_STATUS_MFIR], &endptr, 10);
    mfir.phy_rx_frame_diff = ZING_get_device_status_mfir_phy_rx_frame_diff(endptr);
    
    return mfir;
}

uint16_t ZING_get_device_status_mfir_phy_rx_frame_diff(char* mfir_dst_id_err_diff_end_ptr)
{
    uint16_t mfir_phy_rx_frame_diff = strtol(mfir_dst_id_err_diff_end_ptr + 1, NULL, 10);
    
    return mfir_phy_rx_frame_diff;
}

uint32_t ZING_get_device_status_cnt(uint8_t** device_status)
{
    uint32_t cnt = atoi((char*)device_status[DEVICE_STATUS_CNT]);
    
    return cnt;
}
