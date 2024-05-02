#ifndef AIAS_ZING_H
#define AIAS_ZING_H

#include <stdio.h>
#include <stdlib.h>

#include "project.h"

#define MAX_ZING_STATUS_LENGTH 200
#define MAX_STATUS_VALUE_LENGTH 32
#define MAX_ZING_TIMEOUT_MS 1000

typedef enum
{
    ZCH_USB,
    ZCH_VND,
    ZCH_PRD,
    ZCH_BND,
    ZCH_PPID,
    ZCH_DEVICE_ID,
    ZCH_FMT,
    ZCH_IDX,
    ZCH_TRT,
    ZCH_ACK,
    ZCH_PPC,
    ZCH_TXID,
    ZCH_RXID,
    ZCH_RUN,
    ZCH_CNT,
    NUM_ZCH_STATUS
} ZCH_STATUS_t;

typedef enum
{
    ZCD_USB,
    ZCD_PPID,
    ZCD_DEVICE_ID,
    ZCD_FMT,
    ZCD_IDX,
    ZCD_FPS,
    ZCD_TRT,
    ZCD_ACK,
    ZCD_PPC,
    ZCD_RUN,
    ZCD_ITF,
    ZCD_TXID,
    ZCD_RXID,
    ZCD_DESTID_ERR_CNT,
    ZCD_PHY_RX_FRAME_CNT,
    ZCD_MFIR,
    ZCD_CNT,
    NUM_ZCD_STATUS
} ZCD_STATUS_t;

void ZING_init(void);
uint8_t ZING_get_status(char* zing_status);
uint8_t ZING_parse_zcd_status(char* zing_status, char** status_values);
uint8_t ZING_parse_zch_status(char* zing_status, char** status_values);
uint32_t ZING_parse_status_values(char* status_values, uint8_t type);

#endif