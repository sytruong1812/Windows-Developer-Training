#include "pch.h"
#include "framework.h"
#include "AES.h"
#include "HFACHelper.h"

#pragma comment(lib, "wininet")

bool downloadFileFromURL(const wstring& url, BYTE*& buffer, size_t& sizeBuffer, ERRORS& error)
{
	HINTERNET hInternet = nullptr;
	HINTERNET hUrl = nullptr;
	BYTE* nbuffer = nullptr;
	DWORD status = 0;
	DWORD fileSize = 0;
	DWORD byteRead = 0;
	DWORD totalByteRead = 0;
	DWORD length = sizeof(DWORD);
	DWORD nbufferSize = 100 * MB;
	int dwMaxConnections = MAX_CONNECT;

	hInternet = InternetOpenW(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);
	if (!hInternet) {
		error = ERRORS::INTERNET_INIT_ERROR;
		return false;
	}
	else {
		InternetSetOptionW(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwMaxConnections, sizeof(dwMaxConnections));
	}
	hUrl = InternetOpenUrlW(hInternet, url.data(), NULL, 0, INTERNET_FLAG_RELOAD, NULL);
	if (!hUrl) {
		error = ERRORS::INTERNET_OPEN_ERROR;
		InternetCloseHandle(hInternet);
		return false;
	}
	if (!HttpQueryInfoW(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &length, NULL) ||
		!HttpQueryInfoW(hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &length, NULL))
	{
		if (status == HTTP_STATUS_NOT_FOUND) {
			error = ERRORS::HTTP_404_CODE;
		}
		else {
			error = ERRORS::HTTP_GET_INFO;

		}
		InternetCloseHandle(hUrl);
		return false;
	}
	buffer = new BYTE[fileSize];
	if (!buffer) {
		error = ERRORS::NEW_BUFFER_FALSE;
		InternetCloseHandle(hUrl);
		return false;
	}
	nbuffer = new BYTE[nbufferSize];
	if (!nbuffer) {
		error = ERRORS::NEW_BUFFER_FALSE;
		InternetCloseHandle(hUrl);
		return false;
	}
	while (true) {
		if (InternetReadFile(hUrl, nbuffer, nbufferSize, &byteRead) && byteRead > 0) {
			memcpy(buffer + totalByteRead, nbuffer, byteRead);
			totalByteRead += byteRead;
		}
		else {
			error = ERRORS::INTERNET_READ_FILE;
			break;
		}
	}
	sizeBuffer = totalByteRead;
	if (nbuffer != nullptr) {
		delete[] nbuffer;
		nbuffer = nullptr;
	}
	if (hUrl != nullptr) {
		InternetCloseHandle(hUrl);
	}
	if (hInternet != nullptr) {
		InternetCloseHandle(hInternet);
	}
	return true;
}

bool saveDataToFile(const wstring& outputFile, BYTE* buffer, size_t sizeBuffer, ERRORS& error)
{
	HANDLE hOutputFile = nullptr;
	DWORD bytesWrite = 0;

	hOutputFile = CreateFileW(outputFile.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutputFile == INVALID_HANDLE_VALUE) {
		error = ERRORS::OPEN_FILE_FALSE;
		return false;
	}
	if (!WriteFile(hOutputFile, buffer, static_cast<DWORD>(sizeBuffer), &bytesWrite, NULL) || bytesWrite != sizeBuffer) {
		error = ERRORS::WRITE_FILE_FALSE;
		CloseHandle(hOutputFile);
		return false;
	}
	if (hOutputFile != nullptr) {
		CloseHandle(hOutputFile);
	}
	return true;
}

BYTE* decryptData(BYTE* buffer, size_t& sizeBuffer, BYTE* szKey, ERRORS& error)
{
	AES aes(AESKeyLength::AES_128);
	BYTE iv[16];
	BYTE newKey[16];							// Save value hash key
	aes.generateKeyAndIV(szKey, newKey, iv);
	BYTE* newBuffer = new BYTE[sizeBuffer];		// Save value data decrypted
	if (newBuffer == nullptr) {
		error = ERRORS::NEW_BUFFER_FALSE;
	}
	else {
		memset(newBuffer, 0, sizeBuffer);		// Initialize the buffer with zeros
		aes.decryptCBC(buffer, sizeBuffer, newKey, iv, newBuffer);
		aes.removePadding(newBuffer, sizeBuffer);
	}
	return newBuffer;
}