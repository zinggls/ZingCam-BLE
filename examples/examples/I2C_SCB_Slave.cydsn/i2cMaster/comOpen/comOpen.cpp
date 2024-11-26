#include "pch.h"
#include <iostream>
#include <windows.h>

using namespace std;

static IDispatch* pIDispatch = NULL;

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

	//Use Version Independent Prog ID to instantiate COM-object
	wchar_t progid[] = L"PSoCProgrammerCOM.PSoCProgrammerCOM_Object";

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

cleanup:
	wcout << "Shutting down COM" << endl;
	CoUninitialize();
	return 0;
}