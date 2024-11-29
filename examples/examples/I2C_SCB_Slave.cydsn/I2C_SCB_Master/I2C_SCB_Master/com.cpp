#include "pch.h"
#include "com.h"
#include <atlbase.h>
#include "util.h"

std::wstring CCom::sErrorMsg = L"";

CCom::CCom(const std::wstring& progID)
{
	HRESULT hr = ::CLSIDFromProgID(progID.c_str(), &m_clsid);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to obtain CLSID from ProgID");
	}

	hr = ::CoCreateInstance(m_clsid, NULL, CLSCTX_SERVER, IID_IDispatch, (void**)&m_pIDispatch);
	if (FAILED(hr)) {
		throw std::runtime_error("Failed to create COM instance");
	}

	hr = GetDispIDsByName();
	if (FAILED(hr)) {
		throw std::runtime_error("GetDispIDsByName error");
	}

	hr = ppStartSelfTerminator(GetCurrentProcessId());
	if (FAILED(hr)) {
		throw std::runtime_error("StartSelfTerminator error");
	}
}

CCom::~CCom()
{

}

HRESULT CCom::GetDispIDsByName()
{
	HRESULT hr = S_OK;
	LPOLESTR functionName;

	//_StartSelfTerminator
	functionName = const_cast <LPOLESTR>(L"_StartSelfTerminator");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID__StartSelfTerminator);
	if (FAILED(hr)) return hr;

	//OpenPort
	functionName = const_cast <LPOLESTR>(L"OpenPort");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_OpenPort);
	if (FAILED(hr)) return hr;

	//ClosePort
	functionName = const_cast <LPOLESTR>(L"ClosePort");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_ClosePort);
	if (FAILED(hr)) return hr;

	//GetPorts
	functionName = const_cast <LPOLESTR>(L"GetPorts");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_GetPorts);
	if (FAILED(hr)) return hr;

	//SetPowerVoltage
	functionName = const_cast <LPOLESTR>(L"SetPowerVoltage");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_SetPowerVoltage);
	if (FAILED(hr)) return hr;

	//PowerOn
	functionName = const_cast <LPOLESTR>(L"PowerOn");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_PowerOn);
	if (FAILED(hr)) return hr;

	//SetProtocol
	functionName = const_cast <LPOLESTR>(L"SetProtocol");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_SetProtocol);
	if (FAILED(hr)) return hr;

	//Reset Bus
	functionName = const_cast <LPOLESTR>(L"I2C_ResetBus");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_ResetBus);
	if (FAILED(hr)) return hr;

	//I2C_GetDeviceList
	functionName = const_cast <LPOLESTR>(L"I2C_GetDeviceList");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_GetDeviceList);
	if (FAILED(hr)) return hr;

	//I2C_SetSpeed
	functionName = const_cast <LPOLESTR>(L"I2C_SetSpeed");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_SetSpeed);
	if (FAILED(hr)) return hr;

	//I2C_GetSpeed
	functionName = const_cast <LPOLESTR>(L"I2C_GetSpeed");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_GetSpeed);
	if (FAILED(hr)) return hr;

	//I2C_DataTransfer
	functionName = const_cast <LPOLESTR>(L"I2C_DataTransfer");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_DataTransfer);
	if (FAILED(hr)) return hr;

	//I2C_SendData
	functionName = const_cast <LPOLESTR>(L"I2C_SendData");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_SendData);
	if (FAILED(hr)) return hr;

	//I2C_ReadData
	functionName = const_cast <LPOLESTR>(L"I2C_ReadData");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_ReadData);
	if (FAILED(hr)) return hr;

	//I2C_ReadDataFromReg
	functionName = const_cast <LPOLESTR>(L"I2C_ReadDataFromReg");
	hr = m_pIDispatch->GetIDsOfNames(IID_NULL, &functionName, 1, LOCALE_SYSTEM_DEFAULT, &m_dispID_I2C_ReadDataFromReg);
	if (FAILED(hr)) return hr;

	return hr;
}

long CCom::ppStartSelfTerminator(long ClientProcessID)
{
	DISPID dispid = m_dispID__StartSelfTerminator;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);

	return vaResult.lVal;
}

long CCom::ppGetPorts(std::vector<std::wstring>& portNames, std::wstring& strError)
{
	DISPID dispid = m_dispID_GetPorts;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_OpenPort;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_ClosePort;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_SetPowerVoltage;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_PowerOn;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_SetProtocol;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_I2C_ResetBus;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_I2C_SetSpeed;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_I2C_GetSpeed;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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
	DISPID dispid = m_dispID_I2C_GetDeviceList;
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
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
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

long CCom::ppI2C_DataTransfer(long deviceAddr, long mode, long readLen, std::vector<BYTE> dataIN, std::vector<BYTE>& dataOUT, std::wstring& strError)
{
	DISPID dispid = m_dispID_I2C_DataTransfer;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 6;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);

	//Initialize parameters
	VARIANT dOUT;
	VariantInit(&dOUT);

	//Convert dataIN into SafeArray
	VARIANT dIN;
	ConvertByteVector2SA(dataIN, &dIN);

	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_VARIANT | VT_BYREF;
	dispparams.rgvarg[1].pvarVal = &dOUT;
	dispparams.rgvarg[2].vt = VT_ARRAY | VT_UI1;
	dispparams.rgvarg[2].parray = dIN.parray;
	dispparams.rgvarg[3].vt = VT_I4;
	dispparams.rgvarg[3].lVal = readLen;
	dispparams.rgvarg[4].vt = VT_I4;
	dispparams.rgvarg[4].lVal = mode;
	dispparams.rgvarg[5].vt = VT_I4;
	dispparams.rgvarg[5].lVal = deviceAddr;

	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	ConvertSA2ByteVector(dOUT, dataOUT);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);
	VariantClear(&dOUT);
	VariantClear(&dIN);

	return vaResult.lVal;
}

long CCom::ppI2C_SendData(long deviceAddr, std::vector<BYTE> dataIN, std::wstring& strError)
{
	DISPID dispid = m_dispID_I2C_SendData;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 3;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);

	//Convert dataIN into SafeArray
	VARIANT dIN;
	ConvertByteVector2SA(dataIN, &dIN);

	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_ARRAY | VT_UI1;
	dispparams.rgvarg[1].parray = dIN.parray;
	dispparams.rgvarg[2].vt = VT_I4;
	dispparams.rgvarg[2].lVal = deviceAddr;

	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);
	VariantClear(&dIN);

	return vaResult.lVal;
}

long CCom::ppI2C_ReadData(long deviceAddr, long readLen, std::vector<BYTE>& dataOUT, std::wstring& strError)
{
	DISPID dispid = m_dispID_I2C_ReadData;
	// Set up parameters
	DISPPARAMS dispparams;
	memset(&dispparams, 0, sizeof(DISPPARAMS));
	dispparams.cArgs = 4;
	// Allocate memory for parameters
	VARIANTARG* pArg = new VARIANTARG[dispparams.cArgs];
	dispparams.rgvarg = pArg;
	memset(pArg, 0, sizeof(VARIANT) * dispparams.cArgs);

	//Initialize parameters
	VARIANT dOUT;
	VariantInit(&dOUT);

	BSTR bstrError = 0;
	dispparams.rgvarg[0].vt = VT_BSTR | VT_BYREF;
	dispparams.rgvarg[0].pbstrVal = &bstrError;
	dispparams.rgvarg[1].vt = VT_VARIANT | VT_BYREF;
	dispparams.rgvarg[1].pvarVal = &dOUT;
	dispparams.rgvarg[2].vt = VT_I4;
	dispparams.rgvarg[2].lVal = readLen;
	dispparams.rgvarg[3].vt = VT_I4;
	dispparams.rgvarg[3].lVal = deviceAddr;

	//Init Result (Return Value)
	VARIANTARG vaResult;
	VariantInit(&vaResult);

	HRESULT hr;
	hr = m_pIDispatch->Invoke(dispid, IID_NULL, GetUserDefaultLCID(), DISPATCH_METHOD,
		&dispparams, &vaResult, NULL, NULL);

	USES_CONVERSION;
	strError = BSTRToWString(bstrError);
	ConvertSA2ByteVector(dOUT, dataOUT);
	//Free allocated resources
	delete[] pArg;
	::SysFreeString(bstrError);
	VariantClear(&dOUT);

	return vaResult.lVal;
}

long CCom::OpenPort()
{
	long hr;
	//Open Port - get first MiniProg3 port in the ports list
	std::vector<std::wstring> ports;
	hr = ppGetPorts(ports, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;

	if (ports.size() <= 0) throw std::runtime_error("Connect any Programmer to PC");

	//Port should be opened just once to connect Programmer device (MiniProg1/3,etc).
	//After that you can use Chip-/Programmer- specific APIs as long as you need.
	//No need to repoen port when you need to acquire chip 2nd time, just call Acquire() again.
	//This is true for all other APIs which get available once port is opened.
	//You have to call OpenPort() again if port was closed by ClosePort() method, or 
	//when there is a need to connect to other programmer, or
	//if programmer was physically reconnected to USB-port.

	std::wstring portName = ports[0];
	return CCom::ppOpenPort(portName, CCom::sErrorMsg);
}

long CCom::ClosePort()
{
	return ppClosePort(CCom::sErrorMsg);
}

long CCom::writeI2C(int deviceAddress, int rgb)
{
	std::vector<byte> dataIN;
	dataIN.resize(3);
	dataIN[0] = 0x01;
	dataIN[1] = rgb;
	dataIN[2] = 0x17;
	return CCom::ppI2C_SendData(deviceAddress, dataIN, CCom::sErrorMsg);
}

long CCom::readI2C(int deviceAddress, long readLen, std::vector<BYTE>& dataOUT)
{
	//Read readLen bytes from device
	dataOUT.resize(0);
	return CCom::ppI2C_ReadData(deviceAddress, readLen, dataOUT, CCom::sErrorMsg);
}