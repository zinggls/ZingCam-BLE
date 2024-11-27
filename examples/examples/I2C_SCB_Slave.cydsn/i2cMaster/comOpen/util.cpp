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

void ConvertSA2ByteVector(VARIANT arrayIN, std::vector<BYTE>& vector)
{
	//Extract SA from VARIANT
	SAFEARRAY* psa;
	if (!(arrayIN.vt & VT_ARRAY)) { vector.resize(0); return; }
	if (arrayIN.vt & VT_BYREF)
		psa = *(arrayIN.pparray);
	else
		psa = arrayIN.parray;

	//Convert SA to vector
	long lLBound, lUBound;
	SafeArrayGetLBound(psa, 1, &lLBound);
	SafeArrayGetUBound(psa, 1, &lUBound);
	int size = lUBound - lLBound + 1;
	vector.resize(size);
	long i, j;
	for (j = 0, i = lLBound; i <= lUBound; i++, j++) {
		BYTE b;
		SafeArrayGetElement(psa, &i, &b);
		vector[j] = b;
	}
}