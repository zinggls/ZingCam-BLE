#include "pch.h"
#include <iostream>
#include <windows.h>

using namespace std;

int main()
{
	HRESULT hr = 0;
	cout << "Initializing COM" << endl;
	if (FAILED(CoInitialize(NULL)))
	{
		cout << "Unable to initialize COM" << endl;
		return 0;
	}
	cout << "COM Initialized" << endl;

	cout << "Shutting down COM" << endl;
	CoUninitialize();
	return 0;
}