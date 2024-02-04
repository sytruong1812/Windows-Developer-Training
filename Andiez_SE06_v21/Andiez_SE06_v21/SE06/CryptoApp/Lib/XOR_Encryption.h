#pragma once
#include <windows.h>
#include <string>

#define KB 1024

using namespace std;

void XOR_Crypt(BYTE* buffer, size_t bufferSize, BYTE* key, size_t keySize);

