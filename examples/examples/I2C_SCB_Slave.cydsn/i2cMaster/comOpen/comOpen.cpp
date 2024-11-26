#include "pch.h"
#include <iostream>
#include <windows.h>
#include <vector>
#include <atlbase.h>
#include <comdef.h>

using namespace std;

static IDispatch* pIDispatch = NULL;
static std::wstring sErrorMsg;

//Use Version Independent Prog ID to instantiate COM-object
static const wchar_t progid[] = L"PSoCProgrammerCOM.PSoCProgrammerCOM_Object";

//DispIDs for each function used in the Example
//They are initialized once after COM-object is created by GetDispIDsByName() function
static DISPID dispID__StartSelfTerminator = 0;
static DISPID dispID_OpenPort = 0;
static DISPID dispID_ClosePort = 0;
static DISPID dispID_GetPorts = 0;
static DISPID dispID_SetPowerVoltage = 0;
static DISPID dispID_PowerOn = 0;
static DISPID dispID_SetProtocol = 0;
static DISPID dispID_I2C_ResetBus = 0;
static DISPID dispID_I2C_GetDeviceList = 0;
static DISPID dispID_I2C_SetSpeed = 0;
static DISPID dispID_I2C_GetSpeed = 0;
static DISPID dispID_I2C_DataTransfer = 0;
static DISPID dispID_I2C_SendData = 0;
static DISPID dispID_I2C_ReadData = 0;
static DISPID dispID_I2C_ReadDataFromReg = 0;

int GetDispIDsByName()
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

long ppStartSelfTerminator(long ClientProcessID)
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

std::wstring BSTRToWString(BSTR bstr)
{
	if (bstr == nullptr)
	{
		return L""; // Return an empty string for null BSTR
	}

	// Use _bstr_t for conversion
	_bstr_t bstrWrapper(bstr);
	return static_cast<wchar_t*>(bstrWrapper);
}

long ppGetPorts(std::vector<std::wstring>& portNames, std::wstring& strError)
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

long ppOpenPort(std::wstring portName, std::wstring& strError)
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

long OpenPort()
{
	long hr;
	//Open Port - get first MiniProg3 port in the ports list
	std::vector<std::wstring> ports;
	hr = ppGetPorts(ports, sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;

	if (ports.size() <= 0)
	{
		sErrorMsg = L"Connect any Programmer to PC";
		return -1;
	}

	//Port should be opened just once to connect Programmer device (MiniProg1/3,etc).
	//After that you can use Chip-/Programmer- specific APIs as long as you need.
	//No need to repoen port when you need to acquire chip 2nd time, just call Acquire() again.
	//This is true for all other APIs which get available once port is opened.
	//You have to call OpenPort() again if port was closed by ClosePort() method, or 
	//when there is a need to connect to other programmer, or
	//if programmer was physically reconnected to USB-port.

	std::wstring portName = ports[0];
	wcout << "Port name:" << portName << endl;
	return ppOpenPort(portName, sErrorMsg);
}

long ppClosePort(std::wstring& strError)
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

long ClosePort()
{
	return ppClosePort(sErrorMsg);
}

long Execute()
{
	int hr;

	//Open Port - get last (connected) port in the ports list
	hr = OpenPort();
	if (!SUCCEEDED(hr)) return hr;
	wcout << "OpenPort OK" << endl;

	//Close the Port, so it is available for other apps
	ClosePort();
	wcout << "Port Closed" << endl;
	return hr;
}

int main()
{
	HRESULT hr = 0;
	wcout << "Initializing COM" << endl;
	if (FAILED(CoInitialize(NULL)))
	{
		wcout << "Unable to initialize COM" << endl;
		return 0;
	}
	wcout << "COM Initialized" << endl;


	CLSID clsid;
	hr = ::CLSIDFromProgID(progid, &clsid);
	if (FAILED(hr))
	{
		wcout << "Failed to get class id for PSoC Programmer COM object !" << endl;
		goto cleanup;
	}
	wcout << L"Class ID Obtained from Version Independent Prod ID: " << progid << endl;

	hr = ::CoCreateInstance(clsid, NULL, CLSCTX_SERVER, IID_IDispatch, (void**)&pIDispatch);
	if (FAILED(hr))
	{
		wcout << "Failed to create instance of PSoC Programmer COM object !" << endl;
		goto cleanup;
	}

	hr = GetDispIDsByName();
	if (FAILED(hr))
	{
		cout << "Failed to get DispIDs of used methods";
		CoUninitialize();
		return 0;
	}

	ppStartSelfTerminator(GetCurrentProcessId());

	//Execute actual task of the example
	hr = Execute();

	{
		std::wstring str;
		if (SUCCEEDED(hr))
			str = L"Succeeded!";
		else {
			str = L"Failed! ";
			str.append(sErrorMsg);
		}
		wcout << str.c_str() << endl;
	}

cleanup:
	wcout << "Shutting down COM" << endl;
	CoUninitialize();
	return 0;
}