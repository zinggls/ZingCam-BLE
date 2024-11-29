#pragma once

#include <iostream>
#include <comdef.h>
#include <vector>
#include "ppcom.h"

class CCom {
	CCom();
public:
	CCom(const std::wstring& progID);
	~CCom();

	static std::wstring sErrorMsg;

	long ppStartSelfTerminator(long ClientProcessID);
	long ppGetPorts(std::vector<std::wstring>& portNames, std::wstring& strError);
	long ppOpenPort(std::wstring portName, std::wstring& strError);
	long ppClosePort(std::wstring& strError);
	long ppSetPowerVoltage(std::wstring voltage, std::wstring& strError);
	long ppPowerOn(std::wstring& strError);
	long ppSetProtocol(enumInterfaces protocol, std::wstring& strError);
	long ppI2C_ResetBus(std::wstring& strError);
	long ppI2C_SetSpeed(enumI2Cspeed speed, std::wstring& strError);
	long ppI2C_GetSpeed(long& speed, std::wstring& strError);
	long ppI2C_GetDeviceList(std::vector<BYTE>& devices, std::wstring& strError);
	long ppI2C_DataTransfer(long deviceAddr, long mode, long readLen, std::vector<BYTE> dataIN, std::vector<BYTE>& dataOUT, std::wstring& strError);
	long ppI2C_SendData(long deviceAddr, std::vector<BYTE> dataIN, std::wstring& strError);
	long ppI2C_ReadData(long deviceAddr, long readLen, std::vector<BYTE>& dataOUT, std::wstring& strError);

	long OpenPort();
	long ClosePort();
	long writeI2C(int deviceAddress, std::vector<BYTE>& dataIN);
	long readI2C(int deviceAddress, long readLen, std::vector<BYTE>& dataOUT);

private:
	HRESULT GetDispIDsByName();

private:
	CLSID m_clsid;
	IDispatch *m_pIDispatch;
	DISPID m_dispID__StartSelfTerminator;
	DISPID m_dispID_OpenPort;
	DISPID m_dispID_ClosePort;
	DISPID m_dispID_GetPorts;
	DISPID m_dispID_SetPowerVoltage;
	DISPID m_dispID_PowerOn;
	DISPID m_dispID_SetProtocol;
	DISPID m_dispID_I2C_ResetBus;
	DISPID m_dispID_I2C_GetDeviceList;
	DISPID m_dispID_I2C_SetSpeed;
	DISPID m_dispID_I2C_GetSpeed;
	DISPID m_dispID_I2C_DataTransfer;
	DISPID m_dispID_I2C_SendData;
	DISPID m_dispID_I2C_ReadData;
	DISPID m_dispID_I2C_ReadDataFromReg;
};