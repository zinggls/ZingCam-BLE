#include "operMode.h"

static uint8_t txPrevMode = MODE_PSAVE;
static uint8_t rxPrevMode = MODE_PSAVE;
static ZingRxCallback zingRxCb = NULL;

void setZingRxCallback(ZingRxCallback cb)
{
    zingRxCb = cb;
}

void onWirelessVideoTransmitterOperationMode(uint8_t mode)
{
	if (txPrevMode == MODE_OPER || txPrevMode == MODE_DEFAULT) {        //운용모드
		if (mode == MODE_WAIT) {
			//운용모드->대기모드
			HGATE_Con1_Write(0x04);
			HGATE_Con1_Write(0x00);
		}
		else if (mode == MODE_PSAVE) {
			//운용모드->절전모드
			HGATE_Con1_Write(0x04);
			HGATE_Con1_Write(0x00);
		}

		txPrevMode = mode;
	}
	else if (txPrevMode == MODE_WAIT) {    //대기모드
		if (mode == MODE_OPER || mode == MODE_DEFAULT) {
			//대기모드->운용모드
			HGATE_Con1_Write(0x04);
			HGATE_Con1_Write(0x03);	//송신기 디폴트 출력
		}
		else if (mode == MODE_PSAVE) {
			//대기모드->절전모드
		}

		txPrevMode = mode;
	}
	else if (txPrevMode == MODE_PSAVE) {    //절전모드
		if (mode == MODE_OPER || mode == MODE_DEFAULT) {
			//절전모드->운용모드
			HGATE_Con1_Write(0x04);
			HGATE_Con1_Write(0x03);	//송신기 디폴트 출력
		}
		else if (mode == MODE_WAIT) {
			//절전모드->대기모드
		}

		txPrevMode = mode;
	}
}

void onWirelessVideoReceiverOperationMode(uint8_t mode)
{
	if (rxPrevMode == MODE_OPER || rxPrevMode == MODE_DEFAULT) {        //운용모드
		if (mode == MODE_WAIT) {
			//운용모드->대기모드
			HGATE_Con1_Write(0x04);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x01);
			HGATE_Con1_Write(0x00);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x00);
		}
		else if (mode == MODE_PSAVE) {
			//운용모드->절전모드
			HGATE_Con1_Write(0x04);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x01);
			HGATE_Con1_Write(0x00);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x00);
		}

		rxPrevMode = mode;
	}
	else if (rxPrevMode == MODE_WAIT) {    //대기모드
		if (mode == MODE_OPER || mode == MODE_DEFAULT) {
			//대기모드->운용모드

			//수신기 디폴트 출력
			HGATE_Con1_Write(0x04);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x01);
			HGATE_Con1_Write(0x03);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x03);
		}
		else if (mode == MODE_PSAVE) {
			//대기모드->절전모드
		}

		rxPrevMode = mode;
	}
	else if (rxPrevMode == MODE_PSAVE) {    //절전모드
		if (mode == MODE_OPER || mode == MODE_DEFAULT) {
			//절전모드->운용모드

			//수신기 디폴트 출력
			HGATE_Con1_Write(0x04);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x01);
			HGATE_Con1_Write(0x03);
			HGATE_Con2_1_Write(0x00);
			HGATE_Con2_2_Write(0x03);
		}
		else if (mode == MODE_WAIT) {
			//절전모드->대기모드
		}

		rxPrevMode = mode;
	}
}