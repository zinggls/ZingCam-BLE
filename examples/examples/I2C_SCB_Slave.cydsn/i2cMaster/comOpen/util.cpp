#include "pch.h"
#include "util.h"
#include <comdef.h>

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