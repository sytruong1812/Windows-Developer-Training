#include "RTFFile.h"

bool readFileRTF(const wstring& inputFilename, BYTE*& buffer, size_t& sizeBuffer, FileProcessError& error)
{
	HANDLE hInputFile = nullptr;
	BYTE* bufferTemp = nullptr;
	size_t sizeTemp = 1 * MB;
	size_t num = 0;
	DWORD fileSize = 0;
	DWORD bytesRead = 0;

	hInputFile = CreateFileW(inputFilename.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInputFile == INVALID_HANDLE_VALUE) {
		error = ERROR_OPEN_FILE_IN;
		return false;
	}

	fileSize = GetFileSize(hInputFile, nullptr);
	buffer = new BYTE[fileSize];
	if (buffer == nullptr) {
		error = FAILED_TO_ALLOCATE_MEMORY;
		CloseHandle(hInputFile);
		return false;
	}

	bufferTemp = new BYTE[sizeTemp];
	if (bufferTemp == nullptr) {
		error = FAILED_TO_ALLOCATE_MEMORY;
		CloseHandle(hInputFile);
		return false;
	}

	while (ReadFile(hInputFile, bufferTemp, static_cast<DWORD>(sizeTemp), &bytesRead, NULL) && bytesRead > 0) {
		memcpy(buffer + num, bufferTemp, bytesRead);
		num += bytesRead;
	}
	sizeBuffer = num;
	if (hInputFile != nullptr) {
		CloseHandle(hInputFile);
	}
	if (bufferTemp != nullptr) {
		delete[] bufferTemp;
	}
	bufferTemp = nullptr;
	return true;
}

bool writeFileRTF(const wstring& outputFilename, BYTE* buffer, size_t& sizeBuffer, FileProcessError& error)
{
	HANDLE hOutputFile = nullptr;
	DWORD bytesWrite = 0;

	hOutputFile = CreateFileW(outputFilename.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutputFile == INVALID_HANDLE_VALUE) {
		error = ERROR_OPEN_FILE_OUT;
		return false;
	}

	if (!WriteFile(hOutputFile, buffer, static_cast<DWORD>(sizeBuffer), &bytesWrite, NULL) || bytesWrite != sizeBuffer) {
		error = ERROR_WRITE_FILE;
		CloseHandle(hOutputFile);
		return false;
	}
	if (hOutputFile != nullptr) {
		CloseHandle(hOutputFile);
	}
	return true;
}

