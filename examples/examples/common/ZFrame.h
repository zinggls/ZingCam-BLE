#ifndef Z_FRAME_H
#define Z_FRAME_H

// Define common fields as a macro to be used in both structures
#define COMMON_FIELDS  \
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

#endif /* Z_FRAME_H */