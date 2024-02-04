#include "RC4_Encryption.h"

using namespace std;

void RC4_Crypt(BYTE* buffer, size_t bufferSize, BYTE* key, size_t keySize)
{
	BYTE T[256];
	BYTE S[256];
	BYTE  tmp; // to be used in swaping
	int j = 0, t = 0, i = 0;

	if (keySize < 1 || keySize > 256) {
		return;
	}
	/* S & K initialization */
	for (int i = 0; i < 256; i++)
	{
		S[i] = i;
		T[i] = key[i % keySize];
	}
	/* State Permutation */
	for (size_t i = 0; i < 256; i++)
	{
		j = (j + S[i] + T[i]) % 256;
		// Swap section
		tmp = S[j];
		S[j] = S[i];
		S[i] = tmp;
	}
	j = 0; // reintializing j to reuse it
	for (size_t x = 0; x < bufferSize; x++)
	{
		i = (i + 1) % 256;		// using %256 to avoid exceed the array limit
		j = (j + S[i]) % 256;	// using %256 to avoid exceed the array limit
		// Swap section
		tmp = S[j];
		S[j] = S[i];
		S[i] = tmp;
		t = (S[i] + S[j]) % 256;
		// XOR generated S[t] with Byte from the plaintext / cipher and append each Encrypted/Decrypted byte to result array
		buffer[x] = buffer[x] ^ S[t]; 
	}
}