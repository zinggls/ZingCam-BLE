#include <stdlib.h>

#include "zing.h"
#include "icd.h"

#define NUM_ZING_CNT 10

static uint8_t zing_mode = ZING_MODE_MANUAL;
static uint32_t ZING_parse_systick;
static uint16_t cnt_tmp = 0;
static uint16_t uart_loop = 0;
static uint8_t current_channel = ZING_INFO_CH1;
static uint8_t ZED = 0;
static uint8_t current_itf = 'N';

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
                else
                {
                    return;
                }
            }
        }
        while (ch != ASCII_LF);
    }
    
    UART_ZING_RX_ClearInterrupt();
    
    return;
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

uint8_t ZING_get_ZED(void)
{
    return ZED;
}

void ZING_set_channel_high(void)
{
#if HBLE
    uint8_t arr[4] = { 0x4, 'b', 0x2, 0x1 };
    
    if (current_channel == ZING_INFO_CH1)
    {
        UART_ZING_PutArray(arr, 4);
        current_channel = ZING_INFO_CH2;
#endif
#if DBLE
    if (current_channel == ZING_INFO_CH1)
    {
        RF_SPDT_Write(1);
        current_channel = ZING_INFO_CH2;
#endif
    }
}

void ZING_set_channel_low(void)
{
#if HBLE
    uint8_t arr[4] = { 0x4, 'b', 0x1, 0x1 };
    
    if (current_channel == ZING_INFO_CH2)
    {
        UART_ZING_PutArray(arr, 4);
        current_channel = ZING_INFO_CH1;
#endif
#if DBLE
    if (current_channel == ZING_INFO_CH2)
    {
        RF_SPDT_Write(0);
        current_channel = ZING_INFO_CH1;
#endif
    }
}

uint8_t ZING_get_mode(void)
{
    return zing_mode;
}

void ZING_set_mode(uint8_t value)
{
    zing_mode = value;
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

void ZING_get_status(uint8_t** host_status, uint8_t type, uint8_t index, uint8_t* arg)
{
    uint32_t value;
    
    switch (type)
    {
        case 0: // ASCII
            value = *host_status[index];
            memcpy(arg, &value, sizeof(char));
            break;
        case 1: // HEX, 8-bits
            value = atoi((char*)host_status[index]);
            memcpy(arg, &value, sizeof(char));
            break;
        case 2: // HEX, 16-bits
            value = strtol((char*)host_status[index], NULL, 16);
            memcpy(arg, &value, sizeof(uint16_t));
            break;
        case 3: // HEX, 32-bits
            value = strtol((char*)host_status[index], NULL, 16);
            memcpy(arg, &value, sizeof(uint32_t));
            break;
    }
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

uint32_t ZING_get_device_status_fps(uint8_t** device_status)
{
    uint32_t fps = strtol((char*)device_status[DEVICE_STATUS_FPS], NULL, 16);
    
    return fps;
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

void ZING_set_itf(uint8_t itf)
{
    current_itf = itf;
}

uint8_t ZING_get_itf(void)
{
    return current_itf;
}
