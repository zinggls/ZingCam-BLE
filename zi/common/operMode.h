#pragma operMode

#include <stdint.h>

/*
    0x00 : (default)
    0x01 : 운용모드
    0x02 : 대기모드
    0x04 : 절전모드

    default(0x0) 모드는 운용모드와 동일, 펌웨어 내에서는 모드값 0은 존재하지 않고 운용(0x1),대기(0x2),절전(0x4)값만 정의된다
*/

#define MODE_OPER    (0x01)
#define MODE_WAIT    (0x02)
#define MODE_PSAVE   (0x04)

void onWirelessVideoTransmitterOperationMode(uint8_t mode);
void onWirelessVideoReceiverOperationMode(uint8_t mode);