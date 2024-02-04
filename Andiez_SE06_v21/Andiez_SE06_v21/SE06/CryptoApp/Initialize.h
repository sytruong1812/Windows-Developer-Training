#pragma once
#include "framework.h"

#define KB 1024
#define MB (1024 * 1024)
#define GB (1024 * 1024 * 1024)
#define CHUNK_SIZE (100 * MB)
#define BUFFER_SIZE (16 * MB)

enum FileProcessError
{
	ERROR_OPEN_FILE_IN,
	ERROR_OPEN_FILE_OUT,
	FAILED_TO_ALLOCATE_MEMORY,
	FAILED_TO_READ_FILE,
	FAILED_TO_WRITE_FILE,
	ZERO_BUFFER_SIZE,
	ERROR_ADD_PADDING,
	ERROR_REMOVE_PADDING,
	WARNING_DUB_KEY,
};

const wstring MessagesError[] = {
	L"- Error: Failed to opening input file!",
	L"- Error: Failed to opening output file!",
	L"- Error: Failed to allocate memory!",
	L"- Error: Failed to read data from file!",
	L"- Error: Failed to write data to file!",
	L"- Error: Readable data is 0!",
	L"- Error: Unable to add padding to buffer.",
	L"- Error: Unable to remove padding from buffer.",
	L"- Warning: The key is duplicated, create a new key.",
};

enum Algorithm
{
	XOR_ENCRYPTION,
	XOR_DECRYPTION,
	RC4_ENCRYPTION,
	RC4_DECRYPTION,
	AES_ENCRYPTION,
	AES_DECRYPTION
};

enum Option
{
	ENCRYPTION,
	DECRYPTION
};

struct HandleInfo
{
	HWND hWnd = nullptr;
	HWND hEdit = nullptr;
	Option isEncrypt = ENCRYPTION;
	Algorithm algorithm = XOR_ENCRYPTION;
};

extern TCHAR szFileName[MAX_PATH];
extern TCHAR szFileNameIn[MAX_PATH];
extern TCHAR szFileNameOut[MAX_PATH];
extern BYTE szKey[MAX_PATH];
