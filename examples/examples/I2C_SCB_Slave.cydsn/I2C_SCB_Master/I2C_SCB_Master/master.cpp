#include "pch.h"
#include <iostream>
#include "com.h"
#include <iomanip>

using namespace std;

long Color(CCom& com,int deviceAddress,int rgb)
{
	int hr;

	//w 01 rgb 17
	std::vector<byte> dataIN;
	dataIN.resize(3);
	dataIN[0] = 0x01;
	dataIN[1] = rgb;
	dataIN[2] = 0x17;
	hr = com.writeI2C(deviceAddress, dataIN);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Send CMD_SET: " << rgb << endl;

	//Read 3 bytes from device
	std::vector<byte> dataOUT;
	hr = com.readI2C(deviceAddress, 3, dataOUT);
	if (!SUCCEEDED(hr)) return hr;

	wcout << L"Read " << dataOUT.size() << "bytes from device : ";
	for (size_t i = 0; i < dataOUT.size(); i++) wcout << std::hex << std::setw(2) << std::setfill(L'0') << dataOUT[i] << " ";
	wcout << endl;

	return hr;
}

long Control_I2C_SCB_Slave(CCom& com,int deviceAddress)
{
	int hr;

	while (1) {
		hr = Color(com,deviceAddress, 1);	//RED
		if (!SUCCEEDED(hr)) return hr;

		Sleep(500);

		hr = Color(com,deviceAddress, 2);	//GREEN
		if (!SUCCEEDED(hr)) return hr;

		Sleep(500);

		hr = Color(com,deviceAddress, 3);	//BLUE
		if (!SUCCEEDED(hr)) return hr;

		Sleep(500);

		hr = Color(com,deviceAddress, 0);	//OFF
		if (!SUCCEEDED(hr)) return hr;

		Sleep(500);
	}
	return hr;
}

long Execute(CCom& com)
{
	int hr;

	//Port Initialization
	//Setup Power - "5.0V" and internal
	hr = com.ppSetPowerVoltage(L"5.0", CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Setup Power - 5.0V and internal done" << endl;

	//Power On
	hr = com.ppPowerOn(CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Power On" << endl;

	//Set protocol, connector and frequency
	hr = com.ppSetProtocol(enumInterfaces::I2C, CCom::sErrorMsg); //I2C-protocol
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Set protocol, connector and frequency" << endl;

	//Reset bus
	hr = com.ppI2C_ResetBus(CCom::sErrorMsg);
	wcout << L"Reset bus!" << endl;

	//Sleep script for 100 milliseconds
	Sleep(100);

	//Set I2C speed
	hr = com.ppI2C_SetSpeed(enumI2Cspeed::CLK_100K, CCom::sErrorMsg);
	if (!SUCCEEDED(hr)) return hr;
	wcout << L"Set speed: 100K!" << endl;

	//Get I2C speed
	long speed = 0;
	std::string val = "";
	hr = com.ppI2C_GetSpeed(speed, CCom::sErrorMsg);
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
	hr = com.ppI2C_GetDeviceList(devices, CCom::sErrorMsg);
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

	hr = Control_I2C_SCB_Slave(com,devices[0]);
	if (!SUCCEEDED(hr)) return hr;

	return hr;
}

static std::wstring progID = L"PSoCProgrammerCOM.PSoCProgrammerCOM_Object";

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

	try {
		CCom com(progID);
		wcout << L"Class ID Obtained from Version Independent Prod ID : " << progID << endl;

		hr = com.OpenPort();
		if (!SUCCEEDED(hr)) goto cleanup;
		wcout << L"OpenPort OK" << endl;

		Execute(com);	//Execute actual task of the example

		com.ClosePort();
		wcout << L"Port Closed" << endl;

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
	}
	catch (const std::exception& ex) {
		wcout << L"An exception occurred: " << ex.what() << endl;
	}

cleanup:
	wcout << L"Shutting down COM" << endl;
	CoUninitialize();
exit:
	return 0;
}