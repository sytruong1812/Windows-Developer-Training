#pragma once
#include <string>
#include <Windows.h>
#include "MD5.h"
#include "AES.h"

#define KB 1024				//The size of the array to be allocated is 1 KB
#define MB (1024 * 1024)	//The size of the array to be allocated is 1 MB

using namespace std;

enum ERRORS
{
	ERROR_OPEN_FILE_IN,
	ERROR_OPEN_FILE_OUT,
	NEW_BUFFER_FALSE,
	ZERO_BUFFER_SIZE,
	ERROR_WRITE_FILE,
	ERROR_READ_FILE
};

const wstring Messages[] = {
	L"Error: Failed to opening input file!",
	L"Error: Failed to opening output file!",
	L"Error: Failed to allocate memory!",
	L"Error: Failed to read data from file!",
	L"Error: Failed write data to file!",
	L"Error: Failed read data from file!"
};


/*
- Purpose: This C++ function is used to decrypt data encrypted using the AES (Advanced Encryption Standard) algorithm.
- Systax:
	1. [IN]		wstring		inputFile
	2. [OUT]	BYTE		buffer
	3. [OUT]	size_t		size
	4. [OUT]	enum		error
- Parameters:
	1. inputFile: This is a Unicode string (wstring) that represents the file path you want to read data from.
	2. buffer: This is a reference to the pointer to the BYTE array. Data from the file will be read and saved to this array.
	3. size: This is a reference to the size variable (size_t) representing the size of the data read from the file.
	4. error: This is an enum variable used to capture errors if any error occurs while reading the file.
- Return value: The function returns true if the file reading process is successful, otherwise if there is an error, it assigns the error value to the error variable and returns false.
*/
bool readDataFromFile(const wstring& inputFile, BYTE*& buffer, size_t& size, ERRORS& error);

/*
- Purpose: This function is used to write data from a BYTE array to an already opened file.
- Systax:
	1. [IN]		wstring		outputFile
	2. [IN]		BYTE		buffer
	3. [IN]		size_t		size
	4. [OUT]	enum		error
- Parameters:
	1. outputFile: This is a Unicode string (wstring) that represents the file path to which you want to write data.
	2. buffer: This is a pointer to the BYTE array containing the data to be written to the file.
	3. size: This is a reference to the size variable (size_t) representing the size of the data to write to the file.
	4. error: This is an enum variable used to capture errors if any errors occur during file writing.
- Return value: The function returns true if the file writing process is successful, otherwise if there is an error, it assigns the error value to the error variable and returns false.
*/
bool writeDataToFile(const wstring& outputFile, BYTE* buffer, size_t size, ERRORS& error);

/*
- Purpose: This function is used to encrypt data using the AES (Advanced Encryption Standard) algorithm.
- Systax:
	1. [IN]		BYTE		buffer
	2. [IN]		size_t		size
	3. [IN]		BYTE		key
	4. [OUT]	enum		error
- Parameters:
	1. buffer: This is a reference to a pointer to the BYTE array containing the data that needs to be encrypted.
	2. size: This is a reference to the size variable (size_t) representing the size of the data to be encoded. After the function ends, the value of this variable can change if padding data is added.
	3. key: This is a pointer to a BYTE array containing the key used to encrypt the data.
	4. error: This is an enum variable used to capture errors if any errors occur during encoding.
- Return value: The function returns the BYTE array containing encrypted data.
*/
BYTE* encryptData(BYTE*& buffer, size_t& size, BYTE* key, ERRORS& error);