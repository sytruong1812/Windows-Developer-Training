#pragma once
#include <windows.h>
#include <iostream>
#include <string>

using namespace std;

void RC4_Crypt(BYTE* buffer, size_t bufferSize, BYTE* key, size_t keySize);