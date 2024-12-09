#ifndef BCC_H
#define BCC_H
    
#include "project.h"
#include "ZFrame.h"
    
// Modes for a statemachine
typedef enum SystemMode {
    SM_INITIALIZE,
    SM_SCANNING,
    SM_CONNECTING,
    SM_SERVICEDISCOVERY,
    SM_CONNECTED
} SystemMode_t;

ZXX_FRAME* getZedFrame();
ZXX_FRAME* getZchFrame();
ZCD_FRAME* getZcdFrame();
void CyBle_AppCallback( uint32 eventCode, void *eventParam );
SystemMode_t getSystemMode();
void setPairingState(SystemMode_t m, uint8 *buf);
    
#endif //BCC_H