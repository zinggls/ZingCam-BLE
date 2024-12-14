#ifndef PERIPHERAL_H
#define PERIPHERAL_H
    
#include "ZFrame.h"

#define SCOPE_COMMAND_FIELDS      \
    char scopeKindChangeNotify;   \
    char scopeOutChangeNotify;    \
    char scopeOperMode;           \

#define SCOPE_STATE_INFO_FIELDS   \
    char scopeStateKind;          \
    char scopeStateOut;           \
    char scopeStateBattery;       \
    char scopeStateIR;            \
    char scopeStateEO;            \

#define TX_STATE_INFO_FIELDS      \
    char txStateBattery;          \
    char txStateModem;            \
    char txStateIMU;              \
    
#define IMU_FIELDS                \
    short imu1;                   \
    short imu2;                   \
    short imu3;                   \
    short imu4;                   \
    short imu5;                   \
    short imuChecksum;            \

typedef struct {
    SCOPE_COMMAND_FIELDS
    SCOPE_STATE_INFO_FIELDS
} SCOPE;

typedef struct {
    TX_STATE_INFO_FIELDS
} TX_STATE;

typedef struct {
    IMU_FIELDS
} IMU;

typedef struct {
    SCOPE scope;
    TX_STATE txState;
    IMU imu;
    ZXX_FRAME zxxFrame;
} PERIPHERAL;

#endif /* PERIPHERAL_H */