#pragma once

#include <iostream>
#include <windows.h>
#include <vector>

std::wstring BSTRToWString(BSTR bstr);
void ConvertSA2ByteVector(VARIANT arrayIN, std::vector<BYTE>& vector);
void ConvertByteVector2SA(std::vector<BYTE> vector, VARIANT* arrayOUT);