#ifndef Z_FRAME_H
#define Z_FRAME_H
    
#include <string.h>
#include <stdio.h>
#include <cytypes.h>
#include <stdbool.h>
    
#define ZED_NUM  11
#define ZCH_NUM  15
#define ZCD_NUM  20
    
typedef enum {
    Unknown = 0,
    ZED,
    ZCH,
    ZCD
} ZxxKind;

ZxxKind zxxKind;

// Define common fields as a macro to be used in all structures
#define COMMON_FIELDS   \
    int kind;           \
    int usb;            \
    char bnd;           \
    unsigned int ppid;  \
    unsigned int devid; \
    int fmt;            \
    int idx;            \
    char trt;           \
    char ack;           \
    char ppc;           \
    char run;           \
    unsigned int txid;  \
    unsigned int rxid;  \
    unsigned int cnt;   \
    unsigned int pos;   \

// Define additional fields for ZCD_FRAME
#define ZCD_ADDITIONAL_FIELDS     \
    unsigned int fps;             \
    char itf;                     \
    unsigned int destIdErrCnt;    \
    int destIdDiff;               \
    unsigned int phyRxFrameCnt;   \
    int frameDiff;                \

#define USB_VND_PRD_FIELDS        \
    unsigned int vnd;             \
    unsigned int prd;             \

// Define ZXX_FRAME with additional specific fields
typedef struct {
    COMMON_FIELDS
    USB_VND_PRD_FIELDS
} ZXX_FRAME;

// Define ZCD_FRAME, which includes the common fields and additional ZCD-specific fields
typedef struct {
    COMMON_FIELDS
    ZCD_ADDITIONAL_FIELDS
} ZCD_FRAME;

ZxxKind detectZxx(const char *buf);
ZxxKind inspect(const char *buf);
int parse(void *data, const char *buf);
uint8_t * setZcdBuffer(uint8_t *buf,ZCD_FRAME *zcd);
uint8_t * setZxxBuffer(uint8_t *buf,ZXX_FRAME *z);
bool isNoZingCb(uint32 loopCount,uint32 period,uint32 *zingCount);


#endif /* Z_FRAME_H */