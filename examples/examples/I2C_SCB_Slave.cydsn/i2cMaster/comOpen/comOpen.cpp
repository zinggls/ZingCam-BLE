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

	wcout << "Shutting down COM" << endl;
	CoUninitialize();
	return 0;
}