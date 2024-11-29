#include "pch.h"
#include <iostream>
#include "com.h"
#include <iomanip>
#include "I2C_Control.h"

using namespace std;

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

		std::wstring str;
		if (SUCCEEDED(Execute(com)))
			str = L"Succeeded!";
		else {
			str = L"Failed! ";
			str.append(CCom::sErrorMsg);
		}
		wcout << str.c_str() << endl;

		com.ClosePort();
		wcout << L"Port Closed" << endl;
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