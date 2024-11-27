#include "pch.h"
#include <iostream>
#include "com.h"

using namespace std;

long OpenPort()
{
	long hr;
	//Open Port - get first MiniProg3 port in the ports list
	std::vector<std::wstring> ports;
	hr = CCom::ppGetPorts(ports, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;

	if (ports.size() <= 0)
	{
		CCom::sErrorMsg = L"Connect any Programmer to PC";
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
	return CCom::ppOpenPort(portName, CCom::sErrorMsg);
}

long ClosePort()
{
	return CCom::ppClosePort(CCom::sErrorMsg);
}

long I2C_Operations()
{
	int hr;

	//Port Initialization
	//Setup Power - "5.0V" and internal
	hr = CCom::ppSetPowerVoltage(L"5.0", CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << "Setup Power - 5.0V and internal done" << endl;

	return hr;
}

long Execute()
{
	int hr;

	//Open Port - get last (connected) port in the ports list
	hr = OpenPort();
	if (!SUCCEEDED(hr)) return hr;
	wcout << "OpenPort OK" << endl;

	//Execute I2C communication
	hr = I2C_Operations();

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
		goto exit;
	}
	wcout << "COM Initialized" << endl;


	CLSID clsid;
	hr = ::CLSIDFromProgID(CCom::progid, &clsid);
	if (FAILED(hr))
	{
		wcout << "Failed to get class id for PSoC Programmer COM object !" << endl;
		goto cleanup;
	}
	wcout << L"Class ID Obtained from Version Independent Prod ID: " << CCom::progid << endl;

	hr = ::CoCreateInstance(clsid, NULL, CLSCTX_SERVER, IID_IDispatch, (void**)&CCom::pIDispatch);
	if (FAILED(hr))
	{
		wcout << "Failed to create instance of PSoC Programmer COM object !" << endl;
		goto cleanup;
	}

	hr = CCom::GetDispIDsByName();
	if (FAILED(hr))
	{
		wcout << "Failed to get DispIDs of used methods";
		goto cleanup;
	}

	CCom::ppStartSelfTerminator(GetCurrentProcessId());

	//Execute actual task of the example
	hr = Execute();

	{
		std::wstring str;
		if (SUCCEEDED(hr))
			str = L"Succeeded!";
		else {
			str = L"Failed! ";
			str.append(CCom::sErrorMsg);
		}
		wcout << str.c_str() << endl;
	}

cleanup:
	wcout << "Shutting down COM" << endl;
	CoUninitialize();
exit:
	return 0;
}