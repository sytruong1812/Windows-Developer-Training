#include <iostream>
#include <windows.h>

#define KB 1024

using namespace std;

void createFile(HANDLE fileName, const char* filePath, DWORD option) {
	HANDLE fileName = CreateFile(
		filePath,
		option,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);

	if (fileName == INVALID_HANDLE_VALUE) {
		cout << "Failed to open " << fileName << " file!" << endl;;
	}
	else {
		cout << "Open " << fileName << " file success." << endl;
	}
}

bool copyProc(const char* srcFilePath, const char* desFilePath, int bufferSize, bool overwrite) {
	HANDLE srcFile = CreateFile(
		srcFilePath,
		GENERIC_READ,
		0,
		nullptr,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (srcFile == INVALID_HANDLE_VALUE) {
		cout << "Failed to open source file!" << endl;
		return false;
	}
	else {
		cout << "Open source file success." << endl;
	}

	DWORD option;
	if (overwrite) {
		option = CREATE_ALWAYS;
	}
	else {
		option = CREATE_NEW;
	}

	HANDLE desFile = CreateFile(
		desFilePath,
		GENERIC_WRITE,
		0,
		nullptr,
		option,
		FILE_ATTRIBUTE_NORMAL,
		nullptr
	);
	if (desFile == INVALID_HANDLE_VALUE) {
		cout << "Failed to open destination file!" << endl;
		CloseHandle(srcFile);
		return false;
	}
	else {
		cout << "Open destination file success." << endl;
	}

	DWORD bufferSizeBytes = bufferSize * KB;
	char* buffer = new char[bufferSizeBytes];
	DWORD bytesRead = 0;
	DWORD bytesWrite = 0;

	while (ReadFile(srcFile, buffer, bufferSizeBytes, &bytesRead, NULL) && bytesRead > 0) {
		// Set the file pointer of the destination file to the current position
		SetFilePointer(desFile, 0, nullptr, FILE_CURRENT);

		WriteFile(desFile, buffer, bytesRead, &bytesWrite, NULL);
		if (bytesRead != bytesWrite) {
			cout << "Failed to write to destination file!" << endl;
			CloseHandle(srcFile);
			CloseHandle(desFile);
			delete[] buffer;
			return false;
		}
	}
	CloseHandle(srcFile);
	CloseHandle(desFile);
	delete[] buffer;
	cout << "File copied successfully." << endl;
	return true;
}

int main(int argc, const char* argv[]) {

	if (argc != 5) {
		cout << "Syntax Error!" << endl;
		cout << "<program> <from> <to> <buffer_size> <overwrite>" << endl;
		return 0;
	}
	const char* sourceFilePath = NULL;
	const char* destinationFilePath = NULL;
	char envSourceFile[MAX_PATH];
	char envDestinationFile[MAX_PATH];

	DWORD srcFileLength = GetEnvironmentVariableA(argv[1], envSourceFile, MAX_PATH);
	if (srcFileLength != 0 && srcFileLength < MAX_PATH) {
		sourceFilePath = envSourceFile;
	}
	else {
		sourceFilePath = argv[1];
	}

	DWORD desFileLength = GetEnvironmentVariableA(argv[2], envDestinationFile, MAX_PATH);
	if (desFileLength != 0 && desFileLength < MAX_PATH) {
		destinationFilePath = envDestinationFile;
	}
	else {
		destinationFilePath = argv[2];
	}

	int sizeBuffer = atoi(argv[3]);
	bool option = (strcmp(argv[4], "YES") == 0);

	cout << "*-------Program Copy File-------*" << endl;
	bool result = copyProc(sourceFilePath, destinationFilePath, sizeBuffer, option);
	if (result) {
		cout << "Program executes successfully." << endl;
	}
	else {
		cout << "Program execution failed!" << endl;
	}
	return 0;
}