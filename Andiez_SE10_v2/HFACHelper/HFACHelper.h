#pragma once
#include <string>
#include <Windows.h>
#include <WinInet.h>

#define KB 1024				//The size of the array to be allocated is 1 KB
#define MB (1024 * 1024)	//The size of the array to be allocated is 1 MB
#define MAX_CONNECT 1;		//Specify the number of connections (InternetSetOptionW)

using namespace std;

enum ERRORS
{
	INTERNET_INIT_ERROR,
	INTERNET_OPEN_ERROR,
	HTTP_404_CODE,
	HTTP_GET_INFO,
	NEW_BUFFER_FALSE,
	INTERNET_READ_FILE,
	OPEN_FILE_FALSE,
	WRITE_FILE_FALSE
};

const wstring Messages[] = {
	L"Error unable to initialize Internet connection!",
	L"Error unable to open Internet connection!",
	L"Error could not find information requested by user!",
	L"Error unable to get information from server!",
	L"Error memory allocation failed!",
	L"Error unable to read data from server!",
	L"Error unable to opening output file!",
	L"Error unable write data to file!"
};

/*
- Purpose: This function is used to load a file from a specified URL and save the data to the BYTE array
- Systax:
	1. [IN]		wstring		url
	2. [OUT]	BYTE		buffer
	3. [OUT]	size_t		size
	4. [OUT]	enum		error
- Parameters:
	1. url: This is the input parameter and is a Unicode string (wstring) that represents the URL you want to download the file from. URLs are only supported if they begin with "ftp:", "http:", or "https:".
	2. buffer: This is an output parameter and a pointer to the BYTE array, which in this function will point to the data downloaded from the URL.
	3. size: This is also an output parameter, a variable representing the size of the downloaded data and stored in the buffer array.
	4. error: This is the output parameter and is an enum variable used to capture errors if any errors occur during loading.
- Return value: This function returns a boolean (bool), representing the result of loading the file. If the load is successful, the function returns true, otherwise if there is an error, it returns false.
*/
bool downloadFileFromURL(const wstring& url, BYTE*& buffer, size_t& size, ERRORS& error);

/*
- Purpose: This function is used to save data from a BYTE array to a specified file.
- Systax:
	1. [IN]		wstring		url
	2. [IN]		BYTE		buffer
	3. [IN]		size_t		size
	4. [OUT]	enum		error
- Parameters:
	1. outputFile: This is the input parameter and is a Unicode string (wstring) that represents the file path you want to save the data to.
	2. buffer: This is the input parameter and is a pointer to the BYTE array containing the data you want to save to the file.
	3. size: This is the output parameter, which is a reference to a size variable (size_t) representing the size of the data to save.
	4. error: This is the output parameter and is an enum variable used to capture errors if any errors occur while saving the data to the file.
- Return value: This function returns a boolean value (bool), representing the result of saving data. If saved successfully, the function returns true, otherwise if there is an error, it returns false.
*/
bool saveDataToFile(const wstring& outputFile, BYTE* buffer, size_t size, ERRORS& error);

/*
- Purpose: This C++ function is used to decrypt data encrypted using the AES (Advanced Encryption Standard) algorithm.
- Systax:
	1. [IN]		BYTE		buffer
	2. [IN]		size_t		size
	3. [IN]		BYTE		key
	4. [OUT]	enum		error
- Parameters:
	1. buffer: This is a pointer to the BYTE array containing the data that needs to be decoded.
	2. size: This is a reference to the size variable (size_t) representing the size of the data to decode. After the function ends, this variable can contain the size of the decoded data.
	3. key: This is a pointer to a BYTE array containing the key used to decrypt the data. This key must match the AES encryption algorithm used.
	4. error: This is an enum variable used to capture errors if any errors occur during decoding.
- Return value: The function returns the BYTE array containing the decoded data.
*/
BYTE* decryptData(BYTE* buffer, size_t& size, BYTE* szKey, ERRORS& error);

