#include "pch.h"
#include <iostream>
#include "com.h"
#include <iomanip>

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
	wcout << L"Port name:" << portName << endl;
	return CCom::ppOpenPort(portName, CCom::sErrorMsg);
}

long ClosePort()
{
	return CCom::ppClosePort(CCom::sErrorMsg);
}

long I2C_SCB_Slave(int deviceAddress)
{
	int hr;

	//w 08 01 01 17 p	//Send CMD_SET_RED	
	std::vector<byte> dataIN;
	dataIN.resize(4);
	dataIN[0] = 0x01;
	dataIN[1] = 0x01;
	dataIN[2] = 0x17;
	dataIN[3] = 'p';	//p
	hr = CCom::ppI2C_SendData(deviceAddress, dataIN, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Send CMD_SET_RED" << endl;

	//Read 4 bytes from device
	std::vector<byte> dataOUT;
	dataOUT.resize(0);
	hr = CCom::ppI2C_ReadData(deviceAddress, 4, dataOUT, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;

	wcout << L"Read 4 bytes from device: ";
	for (size_t i = 0; i < dataOUT.size(); i++) wcout << std::hex << std::setw(2) << std::setfill(L'0') << dataOUT[i] << " ";
	wcout << endl;

	return hr;
}

long I2C_Operations()
{
	int hr;

	//Port Initialization
	//Setup Power - "5.0V" and internal
	hr = CCom::ppSetPowerVoltage(L"5.0", CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Setup Power - 5.0V and internal done" << endl;

	//Power On
	hr = CCom::ppPowerOn(CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Power On" << endl;

	//Set protocol, connector and frequency
	hr = CCom::ppSetProtocol(enumInterfaces::I2C, CCom::sErrorMsg); //I2C-protocol
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Set protocol, connector and frequency" << endl;

	//Reset bus
	hr = CCom::ppI2C_ResetBus(CCom::sErrorMsg);
	wcout << L"Reset bus!" << endl;

	//Sleep script for 100 milliseconds
	Sleep(100);

	//Set I2C speed
	hr = CCom::ppI2C_SetSpeed(enumI2Cspeed::CLK_100K, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Set speed: 100K!" << endl;

	//Get I2C speed
	long speed = 0;
	std::string val = "";
	hr = CCom::ppI2C_GetSpeed(speed, CCom::sErrorMsg);
	if (speed == 1) {
		val = "100K";
	}else if (speed == 4) {
		val = "50K";
	}else if (speed == 2) {
		val = "400K";
	}
	wcout << L"Get speed: " << val.c_str() << "!" << endl;
	if (!SUCCEEDED(hr)) return hr;

	//Get device list
	std::vector<byte> devices;
	hr = CCom::ppI2C_GetDeviceList(devices, CCom::sErrorMsg);
	if (!SUCCEEDED(hr))
	{
		CCom::sErrorMsg = L"Failed to enumerate I2C devices";
		return hr;
	}

	//Show devices
	if (devices.size() == 0)
	{
		CCom::sErrorMsg = L"No devices found";
		return hr;
	}
	wcout << L"Devices list:  8bit  7bit" << endl;
	for (size_t i = 0; i < devices.size(); i++)
		wcout << L"     address:  " << std::hex << std::setw(2) << std::setfill(L'0') << (devices[i] << 1) << "    " << std::hex << std::setw(2) << std::setfill(L'0') << devices[i] << endl;

	hr = I2C_SCB_Slave(devices[0]);
	if (!SUCCEEDED(hr)) return hr;

	return hr;
}

long Execute()
{
	int hr;

	//Open Port - get last (connected) port in the ports list
	hr = OpenPort();
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"OpenPort OK" << endl;

	//Execute I2C communication
	hr = I2C_Operations();
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"I2C_Operations OK" << endl;

	//Close the Port, so it is available for other apps
	ClosePort();
	wcout << L"Port Closed" << endl;
	return hr;
}

int main()
{
	HRESULT hr = 0;
	wcout << L"Initializing COM" << endl;
	if (FAILED(CoInitialize(NULL)))
	{
		wcout << L"Unable to initialize COM" << endl;
		goto exit;
	}
	wcout << L"COM Initialized" << endl;


	CLSID clsid;
	hr = ::CLSIDFromProgID(CCom::progid, &clsid);
	if (FAILED(hr))
	{
		wcout << L"Failed to get class id for PSoC Programmer COM object !" << endl;
		goto cleanup;
	}
	wcout << L"Class ID Obtained from Version Independent Prod ID: " << CCom::progid << endl;

	hr = ::CoCreateInstance(clsid, NULL, CLSCTX_SERVER, IID_IDispatch, (void**)&CCom::pIDispatch);
	if (FAILED(hr))
	{
		wcout << L"Failed to create instance of PSoC Programmer COM object !" << endl;
		goto cleanup;
	}

	hr = CCom::GetDispIDsByName();
	if (FAILED(hr))
	{
		wcout << L"Failed to get DispIDs of used methods";
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
	wcout << L"Shutting down COM" << endl;
	CoUninitialize();
exit:
	return 0;
}