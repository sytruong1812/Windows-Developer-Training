#include <iostream>
#include "HFACHelper.h"

int wmain(int argc, const wchar_t* argv[])
{
	ERRORS error;
	wstring url = L"http://localhost:8080/encrypt.txt";
	wstring outputFile = L"C:\\Users\\home\\Documents\\Me\\Resource\\decrypt.txt";

	/*===============================DOWNLOAD===================================*/
	size_t sizeBuffer = 0;
	BYTE* buffer = nullptr;
	if (downloadFileFromURL(url, buffer, sizeBuffer, error) == false) {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	else {
		wcout << "Download file success." << endl;
	}
	/*=============================DECRYPT DATA=================================*/
	BYTE szKey[] = "Key37";
	BYTE* dataDecrypt = decryptData(buffer, sizeBuffer, szKey, error);
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	if (dataDecrypt != nullptr) {
		wcout << "Decrypt data success." << endl;
	}
	else {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	/*==========================SAVE DATA TO FILE===============================*/
	if (!saveDataToFile(outputFile, dataDecrypt, sizeBuffer, error)) {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	else {
		wcout << "Save data to file success." << endl;
	}
	if (dataDecrypt != nullptr) {
		delete[] dataDecrypt;
		dataDecrypt = nullptr;
	}
	return 0;
}