#ifndef ZING_H
#define ZING_H
    
#include <stdio.h>
#include <string.h>

#include "project.h"

#define ASCII_HOST 'H'
#define ASCII_DEVICE 'D'
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
//  HOST_STATUS_TXID,
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
    DEVICE_STATUS_TRT,
    DEVICE_STATUS_ACK,
    DEVICE_STATUS_PPC,
    DEVICE_STATUS_RXID,
    DEVICE_STATUS_RUN,
    DEVICE_STATUS_ITF,
    /*
    DEVICE_STATUS_TXID,
    DEVICE_STATUS_RXID,
    DEVICE_STATUS_DST_ID_ERR_CNT,
    DEVICE_STATUS_PHY_RX_FRAME_CNT,
    DEVICE_STATUS_MFIR,
    */
    DEVICE_STATUS_CNT,
    NUM_DEVICE_STATUS
} ZING_DEVICE_STATUS_NAME;
    
uint8_t** ZING_host_init(void);
uint8_t** ZING_device_init(void);
uint8_t ZING_get_host_status(char* data);
uint8_t ZING_get_device_status(char* data);
uint8_t ZING_parse_host_status(char* device_status, uint8_t** status_values);
uint8_t ZING_parse_device_status(char* device_status, uint8_t** status_values);
void ZING_change_channel(uint8_t** host_status, uint8_t val);

#endif