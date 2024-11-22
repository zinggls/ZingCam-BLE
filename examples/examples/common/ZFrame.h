#ifndef Z_FRAME_H
#define Z_FRAME_H
    
#include <string.h>
#include <stdio.h>
#include <cytypes.h>
    
#define ZED_NUM  11
#define ZCH_NUM  15
    
typedef enum {
    Unknown = 0,
    ZED,
    ZCH
} ZxxKind;

ZxxKind zxxKind = Unknown;

// Define common fields as a macro to be used in both structures
#define COMMON_FIELDS  \
    int kind;          \
    int usb;           \
    char bnd;          \
    unsigned int ppid; \
    unsigned int devid;\
    char trt;          \
    char ack;          \
    char ppc;          \
    unsigned int txid; \
    unsigned int rxid; \
    char run;          \
    unsigned int cnt;  \
    unsigned int pos;  \

// Define ZCH_FRAME with additional specific fields
typedef struct {
    COMMON_FIELDS
    unsigned int vnd;    // VENDOR, e.g., "0xEB1A"
    unsigned int prd;    // PRODUCT, e.g., "0xB101"
    int fmt;             // Format, e.g., "1"
    int idx;             // Index, e.g., "2"
} ZCH_FRAME;

// Define ZED_FRAME which includes only the common fields
typedef struct {
    COMMON_FIELDS
    // No additional fields for ZED_FRAME
} ZED_FRAME;

ZxxKind detectZxx(const char *buf)
{
    if(memcmp(buf,"ZED ",4)==0) return ZED;
    if(memcmp(buf,"ZCH ",4)==0) return ZCH;
    return Unknown;
}

ZxxKind inspect(const char *buf)
{
    int kind = (buf[3]<<24)| (buf[2]<<16) | (buf[1]<<8) | buf[0];
    if(kind==ZED) return ZED;
    if(kind==ZCH) return ZCH;
    return Unknown;
}

int parse(ZxxKind k, void *data, const char *buf)
{
    int rtn = 0;
    
    switch(k) {
        case ZED:
            {
                ZED_FRAME *z = (ZED_FRAME*)data;
                if(ZED_NUM == sscanf(buf, "ZED USB:%d BND:%c PPID:0x%X DeviceID:0x%X TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                                    &z->usb,&z->bnd,&z->ppid,&z->devid,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt)) {
                                        z->kind = ZED;
                                        rtn = 1;
                                    }
            }
            break;
        case ZCH:
            {
                ZCH_FRAME *z = (ZCH_FRAME*)data;
                if(ZCH_NUM == sscanf(buf, "ZCH USB:%d VND:0x%X PRD:0x%X BND:%c PPID:0x%X DeviceID:0x%X FMT:%d IDX:%d TRT:%c ACK:%c PPC:%c TXID:0x%X RXID:0x%X RUN:%c CNT:%d",
                                    &z->usb,&z->vnd,&z->prd,&z->bnd,&z->ppid,&z->devid,&z->fmt,&z->idx,&z->trt,&z->ack,&z->ppc,&z->txid,&z->rxid,&z->run,&z->cnt)) {
                                        z->kind = ZCH;
                                        rtn=1;
                                    }
            }
            break;
        default:
            break;
    }
    return rtn;
}

void *getFrame(ZED_FRAME *zed, ZCH_FRAME *zch)
{
    if(zxxKind==ZED) return zed;
    if(zxxKind==ZCH) return zch;
    return 0;
}

uint16 getFrameSize()
{
    if(zxxKind==ZED) return sizeof(ZED_FRAME);
    if(zxxKind==ZCH) return sizeof(ZCH_FRAME);
    return 0;   //never reach here
}

#endif /* Z_FRAME_H */