#include "Encrypt.h"

using namespace std;

bool readDataFromFile(const wstring& inputFile, BYTE*& rbuffer, size_t& rsize, ERRORS& error)
{
	HANDLE hInputFile = nullptr;
	BYTE* nBuffer = nullptr;
	size_t  sizeTemp = 10 * MB;
	size_t num = 0;
	DWORD fileSize = 0;
	DWORD bytesRead = 0;
	hInputFile = CreateFileW(inputFile.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInputFile == INVALID_HANDLE_VALUE) {
		error = ERROR_OPEN_FILE_IN;
		return false;
	}
	fileSize = GetFileSize(hInputFile, nullptr);
	rbuffer = new BYTE[fileSize];
	if (rbuffer == nullptr) {
		error = NEW_BUFFER_FALSE;
		CloseHandle(hInputFile);
		return false;
	}
	nBuffer = new BYTE[sizeTemp];
	if (nBuffer == nullptr) {
		error = NEW_BUFFER_FALSE;
		CloseHandle(hInputFile);
		return false;
	}
	while (ReadFile(hInputFile, nBuffer, static_cast<DWORD>(sizeTemp), &bytesRead, NULL) && bytesRead > 0) {
		memcpy(rbuffer + num, nBuffer, bytesRead);
		num += bytesRead;
	}
	rsize = num;
	if (hInputFile != nullptr) {
		CloseHandle(hInputFile);
	}
	if (nBuffer != nullptr) {
		delete[] nBuffer;
	}
	nBuffer = nullptr;
	return true;
}

bool writeDataToFile(const wstring& outputFile, BYTE* wbuffer, size_t wsize, ERRORS& error)
{
	HANDLE hOutputFile = nullptr;
	DWORD bytesWrite = 0;
	hOutputFile = CreateFileW(outputFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutputFile == INVALID_HANDLE_VALUE) {
		error = ERROR_OPEN_FILE_OUT;
		return false;
	}
	if (!WriteFile(hOutputFile, wbuffer, static_cast<DWORD>(wsize), &bytesWrite, NULL) || bytesWrite != wsize) {
		error = ERROR_WRITE_FILE;
		CloseHandle(hOutputFile);
		return false;
	}
	if (hOutputFile != nullptr) {
		CloseHandle(hOutputFile);
	}
	return true;
}

BYTE* encryptData(BYTE*& ebuffer, size_t& esize, BYTE* szKey, ERRORS& error)
{
	AES aes(AESKeyLength::AES_128);
	BYTE iv[16];
	BYTE newKey[16];
	aes.generateKeyAndIV(szKey, newKey, iv);
	aes.addPadding(ebuffer, esize);			//Note: Dimension size will be changed here
	BYTE* newBuffer = new BYTE[esize];
	if (newBuffer == nullptr) {
		error = ERRORS::NEW_BUFFER_FALSE;
	}
	else {
		memset(newBuffer, 0, esize);
		aes.encryptCBC(ebuffer, esize, newKey, iv, newBuffer);
	}
	return newBuffer;
}
