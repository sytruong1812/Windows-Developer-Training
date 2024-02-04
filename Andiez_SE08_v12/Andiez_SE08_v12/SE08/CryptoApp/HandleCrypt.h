#pragma once
#include "resource.h"
#include "Initialize.h"

class HandleCrypt
{
private:
	void handleKey(BYTE* key, size_t length, BYTE* newKey);
	void handleUpdateLogs(HWND hWnd, HWND hEdit, const WCHAR* message);
	void handleUpdateLogsHash(HWND hWnd, HWND hEdit, bool isInput, BYTE* digest);
	bool handleCryptByte(HWND hWnd, HWND hEdit, Algorithm option);
	bool handleCryptBlock(HWND hWnd, HWND hEdit, Algorithm option);
	bool createNestedDir(const wstring& path);
	wstring removeLastPathComponent(const wstring& path);
public:
	void handleEncryptDecrypt(HandleInfo* info);
};
