#include "AES.h"

/*
Constructor: AES::AES
Purpose: Based on the provided key length to set the key length (Nk) and number of cycles (Nr)
Comments: None
*/
AES::AES(const AESKeyLength len)
{
	switch (len) {
	case AESKeyLength::AES_128:
		this->Nk = 4;
		this->Nr = 10;
		break;
	case AESKeyLength::AES_192:
		this->Nk = 6;
		this->Nr = 12;
		break;
	case AESKeyLength::AES_256:
		this->Nk = 8;
		this->Nr = 14;
		break;
	}
}

/*
Function: AES::encryptCBC
Purpose: Encrypts a message using the Electronic Codebook (CBC) mode.
Parameter:
	- in[]:
	- inLen[]:
	- key[]:
Comments: Each data block is XORed with the previous block before being encrypted.
	1. checkLength(): Ensures that the length of the input data is divisible by the AES block size.
	2. out: Initializes an input array to store the encrypted data with a size equal to 'inLen'.
	3. block[blockBytesLen]: Initialization Vector (IV).
	4. roundKeys: Initializes an array to store the round keys expanded from the original encryption key.
	5. keyExpansion(): Function to expand the key using 'key' and 'roundKeys' provided.
	6. memcpy(): Copies data from the 'IV' array to the 'block' array.
	7. for(): A loop based on 'inLen' (the length of the input data).
		- xorBlocks(): Executes XOR operation on the input data with the 'block'.
		- encryptBlock(): Performs encryption.
		- memcpy(): Copies data from the 'out' array (from position i to blockBytesLen) to the 'block' array.
	8. delete[] roundKeys: Frees the allocated memory for the 'roundKeys' array and returns the encrypted data array 'out'.
*/
void AES::encryptCBC(const unsigned char in[], size_t inLen, const unsigned char key[], const unsigned char iv[], unsigned char out[])
{
	unsigned char block[blockBytesLen];
	unsigned char* roundKeys = new unsigned char[static_cast<size_t>(4) * Nb * (Nr + static_cast<size_t>(1))];
	keyExpansion(key, roundKeys);
	memcpy(block, iv, blockBytesLen);

	for (size_t i = 0; i < inLen; i += blockBytesLen) {
		xorBlocks(block, in + i, block, blockBytesLen);
		encryptBlock(block, out + i, roundKeys);
		if (inLen >= blockBytesLen) {
			memcpy(block, out + i, blockBytesLen);
		}
	}
	delete[] roundKeys;
}

/*
Function: AES::decryptCBC
Purpose: Decrypts a message using the Electronic Codebook (CBC) mode.
Comments: Similarly to the encryption program, within the for() loop, replace encryptBlock() with decryptBlock() and then perform the xorBlocks() operation.
*/
void AES::decryptCBC(const unsigned char in[], size_t inLen, const unsigned char key[], const unsigned char iv[], unsigned char out[])
{
	unsigned char block[blockBytesLen];
	unsigned char* roundKeys = new unsigned char[static_cast<size_t>(4) * Nb * (Nr + static_cast<size_t>(1))];
	keyExpansion(key, roundKeys);
	memcpy(block, iv, blockBytesLen);

	for (size_t i = 0; i < inLen; i += blockBytesLen) {
		decryptBlock(in + i, out + i, roundKeys);
		xorBlocks(block, out + i, out + i, blockBytesLen);
		if (inLen >= blockBytesLen) {
			memcpy(block, in + i, blockBytesLen);
		}
	}
	delete[] roundKeys;
}

/*
Function: AES::generateIV
Purpose: Using hash function MD5
Comments: None
*/
void AES::generateIV(unsigned char key[], unsigned char iv[])
{
	size_t keyLen = strlen(reinterpret_cast<const char*>(key));
	unsigned char digest[16];		//Save value hash
	MD5One(key, keyLen, digest);
	memcpy(iv, digest, 16);
}

/*
Function: AES::generateKey
Purpose: Using hash function SHA-256
Comments: None
*/
void AES::generateKey(unsigned char key[], unsigned char newKey[])
{
	size_t keyLen = strlen(reinterpret_cast<const char*>(key));
	unsigned char digest[32];		//Save value hash
}

/*
Function: AES::generateKeyAndIV
Purpose: Use the first 8 bytes for the key and the next 8 bytes for the IV
Comments: None
*/
void AES::generateKeyAndIV(unsigned char key[], unsigned char newKey[16], unsigned char iv[16])
{
	size_t keyLen = strlen(reinterpret_cast<const char*>(key));
	unsigned char digest[16];		//Save value hash
	MD5One(key, keyLen, digest);
	memcpy(newKey, digest, 8);
	memcpy(iv, digest + 8, 8);
}

/*
Function: AES::addPadding
Parameter:
	- data: Pointer to data that needs additional padding
	- dataSize: Current size of data in the array
Purpose: PKCS7 (The value of each added byte will be the missing byte to reach the full block size.)
Comments: Let the receiver always know there are padding bytes
	- If dataSize < blockBytesLen: Padding equals the number of missing bytes, data padding value equals the number of missing bytes.
	- If not, consider the next block and check additional padding
	- If dataSize = blockBytesLen: Still padding 16 bytes
*/
void AES::addPadding(unsigned char*& data, size_t& dataSize)
{
	unsigned char* newData = nullptr;
	size_t newDataSize = 0;
	unsigned char paddingValue = 0;
	if (dataSize < blockBytesLen) {
		paddingValue = (unsigned char)(blockBytesLen - dataSize);
	}
	else {
		paddingValue = (unsigned char)(blockBytesLen - (dataSize % blockBytesLen));
		if (paddingValue == 0) {
			paddingValue = blockBytesLen;
		}
	}
	newDataSize = dataSize + paddingValue;
	newData = new unsigned char[newDataSize];
	memcpy(newData, data, dataSize);
	for (size_t i = dataSize; i < newDataSize; ++i) {
		newData[i] = paddingValue;
	}
	delete[] data;
	data = newData;
	dataSize = newDataSize;
}

/*
Function: AES::removePadding
Parameter:
	- data: Pointer to the data to add padding
	- dataSize: The current size of the data in the array
Purpose: PKCS7 (The value of each added byte will be the missing byte to reach the full block size.)
Comments: Remove padding bytes based on the value of the last byte in the decoded data.
*/
void AES::removePadding(unsigned char*& data, size_t& dataSize)
{
	unsigned char* newData = nullptr;
	size_t newDataSize = 0;
	//size_t removePadding = static_cast<size_t>(data[dataSize - 1]);
	unsigned char removePadding = data[dataSize - 1];
	if (dataSize > removePadding) {
		newDataSize = dataSize - removePadding;
		newData = new unsigned char[newDataSize];
		memcpy(newData, data, newDataSize);
		delete[] data;
		data = newData;
		dataSize = newDataSize;
	}
}

/*
Function: AES::subBytes
Parameter: state[4][Nb]: 2-dimensional array containing the AES state (4 rows and Nb columns)
Purpose: Replace each byte in the state with the corresponding value in the S-box table
Comments: None
*/
void AES::subBytes(unsigned char state[4][Nb])
{
	size_t i, j;
	unsigned char t;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			t = state[i][j];						// Get byte value at position i, j in state
			state[i][j] = sbox[t / 16][t % 16];		// Replace byte with corresponding value in S-box
		}
	}
}

/*
Function: AES::shiftRow
Parameter:
	- state[4][Nb]: 2-dimensional array containing the AES state (4 rows and Nb columns)
	- i: Row to be shifted
	- n: Number of positions to shift by
Purpose: Shift row i up by n positions
Comments: None
*/
void AES::shiftRow(unsigned char state[4][Nb], size_t i, size_t n)
{
	unsigned char tmp[Nb];
	for (size_t j = 0; j < Nb; j++) {
		tmp[j] = state[i][(j + n) % Nb];
	}
	memcpy(state[i], tmp, Nb * sizeof(unsigned char));
}

/*
Function: AES::shiftRows
Parameter: state[4][Nb]: 2-dimensional array containing the AES state (4 rows and Nb columns)
Purpose: This function is used to shift rows within the AES state according to a specific rule.
Comments: Call the ShiftRow function three times sequentially to shift rows within the AES state:
	- Shift row 1 to the right by 1 position.
	- Shift row 2 to the right by 2 positions.
	- Shift row 3 to the right by 3 positions.
*/
void AES::shiftRows(unsigned char state[4][Nb])
{
	shiftRow(state, 1, 1);
	shiftRow(state, 2, 2);
	shiftRow(state, 3, 3);
}

/*
Function: AES::xtime
Parameter: b is the byte value to perform multiplication on x (multiply by x).
Purpose: This function performs multiplication of the byte value 'b' by x (performs multiplication by x^1).
Comments: The steps include:
	- (b << 1): Left shift the byte 'b' by 1 bit.
	- ((b >> 7) & 1): Right shift the byte 'b' by 7 bits and perform a bitwise AND with 1 to get the value of the 0th bit.
	- ((b >> 7) & 1) * 0x1b: Then calculate the carry part to be added to the result. If the leftmost bit of 'b'
		+ Equals 1: The value 0x1b will be added to the result.
		+ Equals 0: The value 0 will be added to the result.
	- Finally, these two results are XORed together to add the carry part to the overflow bits of the left shift operation.
*/
unsigned char AES::xtime(unsigned char b)
{
	return (b << 1) ^ (((b >> 7) & 1) * 0x1b);
}

/*
Function: AES::mixColumns
Parameter: state[4][Nb]: 2-dimensional array containing the AES state (4 rows and Nb columns)
Purpose: This function performs a transformation on each column of the state matrix.
Comments: None
*/
void AES::mixColumns(unsigned char state[4][Nb])
{
	unsigned char temp_state[4][Nb];
	for (size_t i = 0; i < 4; ++i) {
		memset(temp_state[i], 0, 4);		// Set all elements in each row of 'temp_state' to 0
	}
	for (size_t i = 0; i < 4; ++i) {
		for (size_t k = 0; k < 4; ++k) {
			for (size_t j = 0; j < 4; ++j) {
				if (CMDS[i][k] == 1) {
					temp_state[i][j] ^= state[k][j];
				}
				else {
					temp_state[i][j] ^= GF_MUL_TABLE[CMDS[i][k]][state[k][j]];
				}
			}
		}
	}
	for (size_t i = 0; i < 4; ++i) {
		memcpy(state[i], temp_state[i], 4);		//Set all elements in each row of 'state' to temp_state[i]
	}
}

/*
Function: AES::addRoundKey
Parameters:
	- state[4][Nb]: 2-dimensional array containing the AES state (4 rows and Nb columns)
	- key: Key array for the current round
Purpose: This function performs XOR between the current state (in the state matrix)
		and the key for the current round (in array form), to generate a new state after each encryption round.
Comments:
	- Utilizes nested loops to iterate through each row and column of the state matrix.
	- At each position, state[i][j] is XORed with the key value at position key[i + 4 * j].
	- The updated result of the above step is then assigned to state[i][j] as the new state.
*/
void AES::addRoundKey(unsigned char state[4][Nb], unsigned char * key)
{
	size_t i, j;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[i][j] = state[i][j] ^ key[i + 4 * j];
		}
	}
}

/*
Function: AES::subWord
Purpose: This function performs the substitution of each byte in a word (4 consecutive bytes)
		with their corresponding values in the AES S-box substitution box.
Comments: Used in the process of key preparation for AES encryption rounds.
*/
void AES::subWord(unsigned char * a)
{
	for (int i = 0; i < 4; i++) {
		a[i] = sbox[a[i] / 16][a[i] % 16];
	}
}

/*
Function: AES::rotWord
Purpose: This function performs the leftward rotation of the bytes in the array 'a' by one position.
Comments: Used in the process of key preparation for AES encryption rounds.
*/
void AES::rotWord(unsigned char * a)
{
	unsigned char c = a[0];
	a[0] = a[1];
	a[1] = a[2];
	a[2] = a[3];
	a[3] = c;
}

/*
Function: AES::xorWords
Purpose: Perform the XOR operation between two sequences of bytes, each being 4 bytes long, and then store the result in a third sequence of bytes.
Comments: None
*/
void AES::xorWords(unsigned char * a, unsigned char * b, unsigned char * c)
{
	size_t i;
	for (i = 0; i < 4; i++) {
		c[i] = a[i] ^ b[i];
	}
}

/*
Function: AES::Rcon
Purpose: Create a "Round Constant" value to be used during key expansion.
Comments: None
*/
void AES::Rcon(unsigned char * a, size_t n)
{
	size_t i;
	unsigned char c = 1;
	for (i = 0; i < n - 1; i++) {
		c = xtime(c);
	}
	a[0] = c;
	a[1] = a[2] = a[3] = 0;
}

/*
Function: AES::invSubBytes
Purpose: Replace each byte in the state with the corresponding value in the inv_sbox table
Comments: None
*/
void AES::invSubBytes(unsigned char state[4][Nb])
{
	size_t i, j;
	unsigned char t;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			t = state[i][j];
			state[i][j] = inv_sbox[t / 16][t % 16];
		}
	}
}

/*
Function: AES::invMixColumns
Purpose: This function performs an inverse transformation on each column of the state matrix.
Comments: None
*/
void AES::invMixColumns(unsigned char state[4][Nb])
{
	unsigned char temp_state[4][Nb];
	for (size_t i = 0; i < 4; ++i) {
		memset(temp_state[i], 0, 4);
	}
	for (size_t i = 0; i < 4; ++i) {
		for (size_t k = 0; k < 4; ++k) {
			for (size_t j = 0; j < 4; ++j) {
				temp_state[i][j] ^= GF_MUL_TABLE[INV_CMDS[i][k]][state[k][j]];
			}
		}
	}
	for (size_t i = 0; i < 4; ++i) {
		memcpy(state[i], temp_state[i], 4);
	}
}

/*
Function: AES::invShiftRows
Purpose: This function performs inverse MixColumns on each column of the AES . state matrix
Comments: None
*/
void AES::invShiftRows(unsigned char state[4][Nb])
{
	shiftRow(state, 1, Nb - 1);
	shiftRow(state, 2, Nb - 2);
	shiftRow(state, 3, Nb - 3);
}

/*
Function: AES::keyExpansion
Purpose: This function performs key expansion to generate round keys.
Comments: AES_256
	- Nk = 8	-> keyLength = 4 * 8 = 32 bytes
	- Nr = 14
	- Nb = 4
*/
void AES::keyExpansion(const unsigned char key[], unsigned char w[])
{
	unsigned char temp[4];
	unsigned char rcon[4];

	size_t i = 0;
	while (i < 4 * Nk) {	// i < 32
		w[i] = key[i];
		i++;
	}

	i = 4 * Nk;
	while (i < 4 * Nb * (Nr + 1)) {		//i = 32 < 60
		temp[0] = w[i - 4 + 0];
		temp[1] = w[i - 4 + 1];
		temp[2] = w[i - 4 + 2];
		temp[3] = w[i - 4 + 3];
		if (i / 4 % Nk == 0) {
			rotWord(temp);
			subWord(temp);
			Rcon(rcon, i / (Nk * 4));
			xorWords(temp, rcon, temp);
		}
		else if (Nk > 6 && i / 4 % Nk == 4) {
			subWord(temp);
		}
		w[i + 0] = w[i - 4 * Nk] ^ temp[0];
		w[i + 1] = w[i + 1 - 4 * Nk] ^ temp[1];
		w[i + 2] = w[i + 2 - 4 * Nk] ^ temp[2];
		w[i + 3] = w[i + 3 - 4 * Nk] ^ temp[3];
		i += 4;
	}
}

/*
Function: AES::encryptBlock
Parameter:
	- in[]: Array containing the data block to be encrypted.
	- out[]: Array containing the data block after encryption.
	- key[]: Key used for encrypting the data block.
Purpose: This function performs the encryption of a block of data.
Comments: None
*/
void AES::encryptBlock(const unsigned char in[], unsigned char out[], unsigned char * roundKeys)
{
	unsigned char state[4][Nb];
	size_t i, j, round;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[i][j] = in[i + 4 * j];
		}
	}
	addRoundKey(state, roundKeys);
	// Nr-1 first cycle: Each cycle consists of 4 successive transformation steps
	for (round = 1; round <= Nr - 1; round++) {
		subBytes(state);
		shiftRows(state);
		mixColumns(state);
		addRoundKey(state, roundKeys + round * static_cast<size_t>(4) * Nb);
	}
	// The Nrth cycle of the mixColumns() operation is skipped
	subBytes(state);
	shiftRows(state);
	addRoundKey(state, roundKeys + Nr * static_cast<size_t>(4) * Nb);
	// After Nr cycles, the data in state[][] is copied to the out[] array, ending the encoding process
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			out[i + 4 * j] = state[i][j];
		}
	}
}

/*
Function: AES::decryptBlock
Parameter:
	- in[]: Array containing the block of data to be decrypted.
	- out[]: Array containing the block of data after decryption.
	- key[]: Key used for decrypting the data block.
Purpose: This function performs the decryption of a block of data.
Comments: None
*/
void AES::decryptBlock(const unsigned char in[], unsigned char out[], unsigned char * roundKeys)
{
	unsigned char state[4][Nb];
	size_t i, j, round;
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			state[i][j] = in[i + 4 * j];
		}
	}
	addRoundKey(state, roundKeys + Nr * static_cast<size_t>(4) * Nb);
	for (round = Nr - 1; round >= 1; round--) {
		invSubBytes(state);
		invShiftRows(state);
		addRoundKey(state, roundKeys + round * static_cast<size_t>(4) * Nb);
		invMixColumns(state);
	}
	invSubBytes(state);
	invShiftRows(state);
	addRoundKey(state, roundKeys);
	for (i = 0; i < 4; i++) {
		for (j = 0; j < Nb; j++) {
			out[i + 4 * j] = state[i][j];
		}
	}
}

/*
Function: AES::xorBlocks
Parameter:
	- a: Pointer to the array containing the first block of data.
	- b: Pointer to the array containing the second block of data.
	- c: Pointer to the array to store the result after performing the XOR operation.
	- len: Length (number of bytes) of the data blocks.
Purpose: This function performs XOR operation between data blocks a and b, and stores the result in data block c.
Comments: None
*/
void AES::xorBlocks(const unsigned char * a, const unsigned char * b, unsigned char * c, size_t len)
{
	for (size_t i = 0; i < len; i++) {
		c[i] = a[i] ^ b[i];
	}
}