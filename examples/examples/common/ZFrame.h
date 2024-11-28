#ifndef Z_FRAME_H
#define Z_FRAME_H
    
#include <string.h>
#include <stdio.h>
#include <cytypes.h>
    
#define ZED_NUM  11
#define ZCH_NUM  15
#define ZCD_NUM  20
    
typedef enum {
    Unknown = 0,
    ZED,
    ZCH,
    ZCD
} ZxxKind;

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

// Define ZCH_FRAME with additional specific fields
typedef struct {
    COMMON_FIELDS
    unsigned int vnd;    // VENDOR, e.g., "0xEB1A"
    unsigned int prd;    // PRODUCT, e.g., "0xB101"
} ZCH_FRAME;

// Define ZED_FRAME which includes only the common fields
typedef struct {
    COMMON_FIELDS
    // No additional fields for ZED_FRAME
} ZED_FRAME;

// Define ZCD_FRAME, which includes the common fields and additional ZCD-specific fields
typedef struct {
    COMMON_FIELDS
    ZCD_ADDITIONAL_FIELDS
} ZCD_FRAME;

ZxxKind detectZxx(const char *buf);
ZxxKind inspect(const char *buf);
int parse(ZxxKind k, void *data, const char *buf);
void *getFrame(ZED_FRAME *zed, ZCH_FRAME *zch);
uint16 getFrameSize();
ZxxKind* getZxxKind();

#endif /* Z_FRAME_H */