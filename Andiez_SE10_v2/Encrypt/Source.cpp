#include <map>
#include <string>
#include <vector>
#include <locale>
#include <codecvt>
#include <iostream>
#include "Encrypt.h"

using namespace std;

void printUsage()
{
	wcout << L"Usage:" << endl;
	wcout << L"  <Encrypt.exe> -input <input_file> -output <output_file> -key <encryption_key>" << endl;
	wcout << L"  -input or -i: Path to the input file." << endl;
	wcout << L"  -output or -o: Path to the output file." << endl;
	wcout << L"  -key or -k: Encryption key." << endl;
	wcout << L"Example:" << endl;
	wcout << L"  Encrypt.exe -input input.data -output output.bin -key mysecretkey" << endl;
}

map<wstring, wstring> parseCommandLine(int argc, const wchar_t* argv[])
{
	map<wstring, wstring> args;
	for (int i = 1; i < argc; i += 2) {
		wstring param = argv[i];
		wstring value = argv[i + 1];
		args[param] = value;
		if (param == L"-input") {
			args[L"-i"] = value;
		}
		else if (param == L"-i") {
			args[L"-input"] = value;
		}
		if (param == L"-output") {
			args[L"-o"] = value;
		}
		else if (param == L"-o") {
			args[L"-output"] = value;
		}
		if (param == L"-key") {
			args[L"-k"] = value;
		}
		else if (param == L"-k") {
			args[L"-key"] = value;
		}
	}
	return args;
}

BYTE* convertWStringToByte(const wstring& wstr)
{
	wstring_convert<codecvt_utf8<wchar_t>> converter;
	string str = converter.to_bytes(wstr);		//Converter wstring to string
	unique_ptr<BYTE[]> ptrSzKey(new BYTE[str.length() + 1]);	// Use smart pointer unique_ptr for automatic memory management
	memcpy(ptrSzKey.get(), str.c_str(), str.length() + 1);		// Copy data from string to unique_ptr using memcpy
	BYTE * cvKey = ptrSzKey.release();		// Release the ownership of the memory by transferring it to a raw pointer
	return cvKey;
}

int wmain(int argc, const wchar_t* argv[])
{
	if (argc != 7) {
		wcout << L"Syntax Error!" << endl;
		printUsage();
		return 0;
	}
	map<wstring, wstring> args = parseCommandLine(argc, argv);
	wstring input = args[L"-input"];
	wstring output = args[L"-output"];
	wstring szKey = args[L"-key"];
	/*==============================READ FILE=================================*/
	ERRORS error;
	BYTE* buffer = nullptr;
	size_t sizeBuffer = 0;
	if (!readDataFromFile(input, buffer, sizeBuffer, error)) {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	else {
		wcout << "Read data from file success." << endl;
	}
	/*===============================ENCRYPT==================================*/
	BYTE* key = convertWStringToByte(szKey);
	size_t encryptedSize = sizeBuffer;		//Note: Dimension size will be changed here
	BYTE* dataEncrypt = encryptData(buffer, encryptedSize, key, error);
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	if (dataEncrypt != nullptr) {
		wcout << "Encrypt data success." << endl;
	}
	else {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	/*==========================SAVE DATA TO FILE===============================*/
	if (!writeDataToFile(output, dataEncrypt, encryptedSize, error)) {
		wcout << Messages[error].c_str() << endl;
		return 0;
	}
	else {
		wcout << "Write data to file success." << endl;
	}
	if (dataEncrypt != nullptr) {
		delete[] dataEncrypt;
		dataEncrypt = nullptr;
	}
	return 0;
}