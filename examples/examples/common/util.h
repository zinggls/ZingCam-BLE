#pragma once

#include <iostream>
#include <windows.h>
#include <vector>

std::wstring BSTRToWString(BSTR bstr);
void ConvertSA2ByteVector(VARIANT arrayIN, std::vector<BYTE>& vector);
void ConvertByteVector2SA(std::vector<BYTE> vector, VARIANT* arrayOUT);
int ToInt(const std::vector<byte>& data, size_t startIndex);
CString ToHex(const std::vector<byte>& data, size_t startIndex);
CString ToIntStr(const std::vector<byte>& data, size_t startIndex);
CString ToCharStr(const std::vector<byte>& data, size_t startIndex);
CString ToHexStr(const std::vector<byte>& data, size_t startIndex, size_t count);