#ifndef ZING_H
#define ZING_H
    
#include <stdio.h>
#include <string.h>

#include "project.h"

#define ASCII_HOST 'Z'
#define ASCII_DEVICE 'Z'
#define ASCII_LF '\n'
#define MAX_BUFFER_LENGTH 256
#define MAX_VALUE_LENGTH 32
    
typedef enum { 
    ZED_STATUS_USB,
    //ZED_STATUS_VND,
    //ZED_STATUS_PRD,
    ZED_STATUS_BND,
    ZED_STATUS_PPID,
    ZED_STATUS_DEVID,
    //ZED_STATUS_FMT,
    //ZED_STATUS_IDX,
    ZED_STATUS_TRT,
    ZED_STATUS_ACK,
    ZED_STATUS_PPC,
    ZED_STATUS_TXID,
    ZED_STATUS_RXID,
    ZED_STATUS_RUN,
    ZED_STATUS_CNT,
    NUM_ZED_STATUS,
} ZED_STATUS_NAME;
    
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

typedef enum
{
    ZING_MODE_UNKNOWN,
    ZING_MODE_AUTO,
    ZING_MODE_MANUAL,
} ZING_MODE;

typedef enum
{
    ZING_INFO_DEFAULT,
    ZING_INFO_CH1,
    ZING_INFO_CH2,
} ZING_INFO;

typedef struct
{
    uint32_t cnt;
    uint16_t diff;
} ZING_diff;

typedef struct
{
    uint16_t dst_id_err_diff;
    uint16_t phy_rx_frame_diff;
} ZING_mfir; 

CY_ISR(UART_ZING_RX_INTERRUPT);

uint8_t** ZING_host_init(void);
uint8_t** ZING_device_init(void);
uint8_t ZING_get_host_status(char* data);
uint8_t ZING_get_device_status(char* data);
uint8_t ZING_parse_host_status(uint8_t** status_values);
uint8_t ZING_parse_device_status(uint8_t** status_values);
uint8_t ZING_get_ZED(void);
void ZING_set_channel_high(void);
void ZING_set_channel_low(void);
uint8_t ZING_get_mode(void);
void ZING_set_mode(uint8_t mode);
uint8_t ZING_get_info(void);
void ZING_reset(void);

void ZING_get_status(uint8_t** host_status, uint8_t type, uint8_t index, uint8_t* arg);
uint8_t ZING_get_host_status_usb(uint8_t** host_status);
uint16_t ZING_get_host_status_vnd(uint8_t** host_status);
uint16_t ZING_get_host_status_prd(uint8_t** host_status);
char ZING_get_host_status_bnd(uint8_t** host_status);
uint16_t ZING_get_host_status_ppid(uint8_t** host_status);
uint16_t ZING_get_host_status_devid(uint8_t** host_status);
uint8_t ZING_get_host_status_fmt(uint8_t** host_status);
uint8_t ZING_get_host_status_idx(uint8_t** host_status);
char ZING_get_host_status_trt(uint8_t** host_status);
char ZING_get_host_status_ack(uint8_t** host_status);
char ZING_get_host_status_ppc(uint8_t** host_status);
uint32_t ZING_get_host_status_txid(uint8_t** host_status);
uint32_t ZING_get_host_status_rxid(uint8_t** host_status);
char ZING_get_host_status_run(uint8_t** host_status);
uint32_t ZING_get_host_status_cnt(uint8_t** host_status);
uint8_t ZING_get_device_status_usb(uint8_t** device_status);
uint16_t ZING_get_device_status_ppid(uint8_t** device_status);
uint16_t ZING_get_device_status_devid(uint8_t** device_status);
uint8_t ZING_get_device_status_fmt(uint8_t** device_status);
uint8_t ZING_get_device_status_idx(uint8_t** device_status);
uint32_t ZING_get_device_status_fps(uint8_t** device_status);
char ZING_get_device_status_trt(uint8_t** device_status);
char ZING_get_device_status_ack(uint8_t** device_status);
char ZING_get_device_status_ppc(uint8_t** device_status);
char ZING_get_device_status_run(uint8_t** device_status);
char ZING_get_device_status_itf(uint8_t** device_status);
uint32_t ZING_get_device_status_txid(uint8_t** device_status);
uint32_t ZING_get_device_status_rxid(uint8_t** device_status);
ZING_diff ZING_get_device_status_dst_id_err_cnt(uint8_t** device_status);
ZING_diff ZING_get_device_status_phy_rx_frame_cnt(uint8_t** device_status);
ZING_mfir ZING_get_device_status_mfir(uint8_t** device_status);
uint32_t ZING_get_device_status_cnt(uint8_t** device_status);
void ZING_set_itf(uint8_t itf);
uint8_t ZING_get_itf(void);

#endif