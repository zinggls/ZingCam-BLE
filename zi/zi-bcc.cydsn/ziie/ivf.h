#pragma once

#define ICD_COMMAND_SIZE	13
#define ICD_SIZE			25
#define IMU_TX_SIZE			12
#define IMU_RX_SIZE			12
#define ICD_IVF_SIZE		(ICD_SIZE+IMU_TX_SIZE+IMU_RX_SIZE)		//ICD_SIZE + IMU_TX_SIZE + IMU_RX_SIZE = 25 + 12 + 12 = 49
#define ZING_ZXX_SIZE		53										//sizeof(ZXX_FRAME)
#define ZING_ZCD_SIZE		66										//sizeof(ZCD_FRAME)
#define IMU_RX_OFFSET		(ICD_SIZE+IMU_TX_SIZE)					//ICD_SIZE + IMU_TX_SIZE = 25 + 12 = 37

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
	byte TxPower;
	byte RxPower;
} I2C_IVF_COMMAND;

typedef struct I2C_STATE {
	byte ScopeStateKind;
	byte ScopeStateOut;
	byte TxStateLmscopeDetect;
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

	//USB_VND_PRD_FIELDS
	unsigned int vnd;
	unsigned int prd;
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