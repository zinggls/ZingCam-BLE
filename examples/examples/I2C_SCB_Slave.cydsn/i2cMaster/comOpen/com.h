#pragma once

#include <iostream>
#include <comdef.h>
#include <vector>

class CCom {
public:
	CCom();
	~CCom();

	static const wchar_t progid[];
	static std::wstring sErrorMsg;
	static IDispatch* pIDispatch;
	static DISPID dispID__StartSelfTerminator;
	static DISPID dispID_OpenPort;
	static DISPID dispID_ClosePort;
	static DISPID dispID_GetPorts;
	static DISPID dispID_SetPowerVoltage;
	static DISPID dispID_PowerOn;
	static DISPID dispID_SetProtocol;
	static DISPID dispID_I2C_ResetBus;
	static DISPID dispID_I2C_GetDeviceList;
	static DISPID dispID_I2C_SetSpeed;
	static DISPID dispID_I2C_GetSpeed;
	static DISPID dispID_I2C_DataTransfer;
	static DISPID dispID_I2C_SendData;
	static DISPID dispID_I2C_ReadData;
	static DISPID dispID_I2C_ReadDataFromReg;

	static int GetDispIDsByName();
	static long ppStartSelfTerminator(long ClientProcessID);
	static long ppGetPorts(std::vector<std::wstring>& portNames, std::wstring& strError);
	static long ppOpenPort(std::wstring portName, std::wstring& strError);
	static long ppClosePort(std::wstring& strError);
	static long ppSetPowerVoltage(std::wstring voltage, std::wstring& strError);
};