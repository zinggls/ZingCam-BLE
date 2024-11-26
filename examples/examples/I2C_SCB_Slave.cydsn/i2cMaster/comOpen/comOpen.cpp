#include "pch.h"
#include <iostream>
#include <windows.h>

using namespace std;

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

cleanup:
	wcout << "Shutting down COM" << endl;
	CoUninitialize();
	return 0;
}