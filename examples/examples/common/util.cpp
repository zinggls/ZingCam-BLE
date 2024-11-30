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

void ConvertByteVector2SA(std::vector<BYTE> vector, VARIANT* arrayOUT)
{
	SAFEARRAY* psa;
	SAFEARRAYBOUND rgsabound[1];
	rgsabound[0].lLbound = 0;
	rgsabound[0].cElements = (ULONG)vector.size();
	psa = SafeArrayCreate(VT_UI1, 1, rgsabound);
	for (UINT i = 0; i < vector.size(); i++) {
		SafeArrayPutElement(psa, (long*)&i, &vector[i]);
	}

	//return data in VARIANT
	VariantInit(arrayOUT);
	V_VT(arrayOUT) = VT_ARRAY | VT_UI1;
	V_ARRAY(arrayOUT) = psa;
}

int ToInt(const std::vector<byte>& data, size_t startIndex)
{
	// Ensure the startIndex is valid and we have at least 4 bytes to read
	if ((startIndex + 3) >= data.size()) throw std::out_of_range("Not enough data in vector to read 4 bytes.");

	// Convert little-endian bytes to integer
	return static_cast<int>(
		static_cast<uint32_t>(data[startIndex]) |
		(static_cast<uint32_t>(data[startIndex + 1]) << 8) |
		(static_cast<uint32_t>(data[startIndex + 2]) << 16) |
		(static_cast<uint32_t>(data[startIndex + 3]) << 24)
		);
}