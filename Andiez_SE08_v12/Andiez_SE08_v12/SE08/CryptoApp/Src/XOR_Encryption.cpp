#include "XOR_Encryption.h"

using namespace std;

void XOR_Crypt(BYTE* buffer, size_t bufferSize, BYTE* key, size_t keySize)
{
	for (size_t i = 0; i < bufferSize; ++i) {
		buffer[i] = buffer[i] ^ static_cast<BYTE>(key[i % keySize]);
	}
}