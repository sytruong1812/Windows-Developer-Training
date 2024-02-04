#pragma once
#include <iostream>
#include <windows.h>

#define KB 1024
#define MB (1024 * 1024)

using namespace std;

enum FileProcessError
{
	ERROR_OPEN_FILE_IN,
	ERROR_OPEN_FILE_OUT,
	FAILED_TO_ALLOCATE_MEMORY,
	ZERO_BUFFER_SIZE,
	ERROR_WRITE_FILE
};

const wstring MessagesError[] = {
	L"- Error: Failed to opening input file!",
	L"- Error: Failed to opening output file!",
	L"- Error: Failed to allocate memory!",
	L"- Error: Failed to read data from file!",
	L"- Error: Failed write data to file!"
};

bool readFileRTF(const wstring& inputFilename, BYTE*& buffer, size_t& sizeBuffer, FileProcessError& error);
bool writeFileRTF(const wstring& outputFilename, BYTE* buffer, size_t& sizeBuffer, FileProcessError& error);