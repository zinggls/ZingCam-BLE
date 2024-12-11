#pragma once

typedef struct I2C_IVF_COMMAND {
	byte ScopeKind;
	byte ScopeOut;
	byte WirelessChannelMode;
	byte WirelessChannelInfo;
	byte OpmodeScope;
	byte OpmodeTx;
	byte OpmodeRx;
	byte TxImuType;
	byte TxImuCalib;
	byte RxImuType;
	byte RxImuCalib;
} I2C_IVF_COMMAND;

typedef struct I2C_STATE {
	byte ScopeStateKind;
	byte ScopeStateOut;
	byte ScopeStateBattery;
	byte ScopeStateIR;
	byte ScopeStateEO;
	byte TxStateBattery;
	byte TxStateModem;
	byte TxStateImu;
	byte RxStateModem;
	byte RxStateImu;
	byte BleState;
} I2C_STATE;

typedef struct IMU {
	short data1;
	short data2;
	short data3;
	short data4;
	short data5;
	short checksum;
} IMU;

typedef struct ZING_BASE {
	int kind;
	int usb;
	char bnd;
	unsigned int ppid;
	unsigned int devid;
	int fmt;
	int idx;
	char trt;
	char ack;
	char ppc;
	char run;
	unsigned int txid;
	unsigned int rxid;
	unsigned int cnt;
	unsigned int pos;
} ZING_BASE;

typedef struct ZXX {
	ZING_BASE zb;

	//IMU_FIELDS (��������۽ű��� IMU �����͵�� BLE�� ���� ���ű�� ���޵Ǵ� �����͵�)
	IMU txImu;

	//USB_VND_PRD_FIELDS
	unsigned int vnd;
	unsigned int prd;

	//SCOPE_STATE_INFO_FIELDS
	char scopeStateKind;
	char scopeStateOut;
	char scopeStateBattery;
	char scopeStateIR;
	char scopeStateEO;

	//TX_STATE_INFO_FIELDS
	char txStateBattery;
	char txStateModem;
	char txStateIMU;
} ZXX;

typedef struct ZCD {
	ZING_BASE zb;

	unsigned int fps;
	char itf;
	unsigned int destIdErrCnt;
	int destIdDiff;
	unsigned int phyRxFrameCnt;
	int frameDiff;
} ZCD;

typedef struct IVF {
	I2C_IVF_COMMAND read;
	I2C_IVF_COMMAND write;
	I2C_STATE state;
	IMU txImu;
	IMU rxImu;
	ZXX zxx;
	ZCD zcd;
} IVF;