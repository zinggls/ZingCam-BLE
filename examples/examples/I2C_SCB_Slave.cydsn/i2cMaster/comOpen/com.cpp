#include "pch.h"
#include "com.h"
#include <atlbase.h>
#include "util.h"

const wchar_t CCom::progid[] = L"PSoCProgrammerCOM.PSoCProgrammerCOM_Object";
std::wstring CCom::sErrorMsg = L"";
IDispatch* CCom::pIDispatch = 0;
DISPID CCom::dispID__StartSelfTerminator = 0;
DISPID CCom::dispID_OpenPort = 0;
DISPID CCom::dispID_ClosePort = 0;
DISPID CCom::dispID_GetPorts = 0;
DISPID CCom::dispID_SetPowerVoltage = 0;
DISPID CCom::dispID_PowerOn = 0;
DISPID CCom::dispID_SetProtocol = 0;
DISPID CCom::dispID_I2C_ResetBus = 0;
DISPID CCom::dispID_I2C_GetDeviceList = 0;
DISPID CCom::dispID_I2C_SetSpeed = 0;
DISPID CCom::dispID_I2C_GetSpeed = 0;
DISPID CCom::dispID_I2C_DataTransfer = 0;
DISPID CCom::dispID_I2C_SendData = 0;
DISPID CCom::dispID_I2C_ReadData = 0;
DISPID CCom::dispID_I2C_ReadDataFromReg = 0;

CCom::CCom()
{

}

CCom::~CCom()
{

}

int CCom::GetDispIDsByName()
{
	int hr = S_OK;
	LPOLESTR functionName;

	//_StartSelfTerminator
	functionName = const_cast <LPOLESTR>(L"_StartSelfTerminator");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID__StartSelfTerminator);
	if (FAILED(hr)) return hr;

	//OpenPort
	functionName = const_cast <LPOLESTR>(L"OpenPort");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_OpenPort);
	if (FAILED(hr)) return hr;

	//ClosePort
	functionName = const_cast <LPOLESTR>(L"ClosePort");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_ClosePort);
	if (FAILED(hr)) return hr;

	//GetPorts
	functionName = const_cast <LPOLESTR>(L"GetPorts");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_GetPorts);
	if (FAILED(hr)) return hr;

	//SetPowerVoltage
	functionName = const_cast <LPOLESTR>(L"SetPowerVoltage");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_SetPowerVoltage);
	if (FAILED(hr)) return hr;

	//PowerOn
	functionName = const_cast <LPOLESTR>(L"PowerOn");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_PowerOn);
	if (FAILED(hr)) return hr;

	//SetProtocol
	functionName = const_cast <LPOLESTR>(L"SetProtocol");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_SetProtocol);
	if (FAILED(hr)) return hr;

	//Reset Bus
	functionName = const_cast <LPOLESTR>(L"I2C_ResetBus");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_ResetBus);
	if (FAILED(hr)) return hr;

	//I2C_GetDeviceList
	functionName = const_cast <LPOLESTR>(L"I2C_GetDeviceList");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_GetDeviceList);
	if (FAILED(hr)) return hr;

	//I2C_SetSpeed
	functionName = const_cast <LPOLESTR>(L"I2C_SetSpeed");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_SetSpeed);
	if (FAILED(hr)) return hr;

	//I2C_GetSpeed
	functionName = const_cast <LPOLESTR>(L"I2C_GetSpeed");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_GetSpeed);
	if (FAILED(hr)) return hr;

	//I2C_DataTransfer
	functionName = const_cast <LPOLESTR>(L"I2C_DataTransfer");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_DataTransfer);
	if (FAILED(hr)) return hr;

	//I2C_SendData
	functionName = const_cast <LPOLESTR>(L"I2C_SendData");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_SendData);
	if (FAILED(hr)) return hr;

	//I2C_ReadData
	functionName = const_cast <LPOLESTR>(L"I2C_ReadData");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_ReadData);
	if (FAILED(hr)) return hr;

	//I2C_ReadDataFromReg
	functionName = const_cast <LPOLESTR>(L"I2C_ReadDataFromReg");
	hr = pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &dispID_I2C_ReadDataFromReg);
	if (FAILED(hr)) return hr;

	return hr;
}

long CCom::ppStartSelfTerminator(long ClientProcessID)
{
	DISPID dispid = dispID__StartSelfTerminator;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 1;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_I4;
	dispparams.rgvarg[0].lVal = ClientProcessID;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppGetPorts(std::vector<std::wstring>& portNames, std::wstring& strError)
{
	DISPID dispid = dispID_GetPorts;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	VARIANT varPorts;
	VariantInit(&varPorts);
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_VARIANT | VT_BYREF;
	dispparams.rgvarg[1].pvarVal = &varPorts;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);
	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Translate Result into std::vector<std::string>
	long lLBound, lUBound;
	SAFEARRAY* psa = varPorts.parray;
	SafeArrayGetLBound(psa, 1, &lLBound);
	SafeArrayGetUBound(psa, 1, &lUBound);
	int size = lUBound - lLBound + 1;
	portNames.resize(size);
	long i, j;
	for (j = 0, i = lLBound; i <= lUBound; i++, j++) {
		BSTR bPort;
		SafeArrayGetElement(psa, &i, &bPort);
		USES_CONVERSION;
		portNames[j] = BSTRToWString(bPort);
	}
	//Free allocated resources
	delete[] pArg;
	VariantClear(&varPorts);
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppOpenPort(std::wstring portName, std::wstring& strError)
{
	DISPID dispid = dispID_OpenPort;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	USES_CONVERSION;
	BSTR bstrPort = ::SysAllocString(portName.c_str());
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_BSTR;
	dispparams.rgvarg[1].bstrVal = bstrPort;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);


	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrPort);
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppClosePort(std::wstring& strError)
{
	DISPID dispid = dispID_ClosePort;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 1;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppSetPowerVoltage(std::wstring voltage, std::wstring& strError)
{
	DISPID dispid = dispID_SetPowerVoltage;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	USES_CONVERSION;
	BSTR bstrVoltage = ::SysAllocString(voltage.c_str());
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_BSTR;
	dispparams.rgvarg[1].bstrVal = bstrVoltage;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrVoltage);
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppPowerOn(std::wstring& strError)
{
	DISPID dispid = dispID_PowerOn;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 1;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppSetProtocol(enumInterfaces protocol, std::wstring& strError)
{
	DISPID dispid = dispID_SetProtocol;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_I4;
	dispparams.rgvarg[1].lVal = protocol;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppI2C_ResetBus(std::wstring& strError)
{
	DISPID dispid = dispID_I2C_ResetBus;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 1;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppI2C_SetSpeed(enumI2Cspeed speed, std::wstring& strError)
{
	DISPID dispid = dispID_I2C_SetSpeed;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_I4;
	dispparams.rgvarg[1].lVal = speed;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppI2C_GetSpeed(long& speed, std::wstring& strError)
{
	DISPID dispid = dispID_I2C_GetSpeed;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_I4 | VT_BYREF;
	dispparams.rgvarg[1].plVal = &speed;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppI2C_GetDeviceList(std::vector<BYTE>& devices, std::wstring& strError)
{
	DISPID dispid = dispID_I2C_GetDeviceList;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 2;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);
	//Initialize parameters
	VARIANT varDevices;
	VariantInit(&varDevices);
	BSTR bstrError = 0;

	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_VARIANT | VT_BYREF;
	dispparams.rgvarg[1].pvarVal = &varDevices;
	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	ConvertSA2ByteVector(varDevices, devices);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);
	VariantClear(&varDevices);

	return vaResult.lVal;
}