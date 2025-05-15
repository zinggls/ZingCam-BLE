#include "operMode.h"

static uint8_t prevMode = MODE_OPER;
static ZingRxCallback zingRxCb = NULL;

void setZingRxCallback(ZingRxCallback cb)
{
    zingRxCb = cb;
}

void onWirelessVideoTransmitterOperationMode(uint8_t mode)
{
}

void onWirelessVideoReceiverOperationMode(uint8_t mode)
{
}