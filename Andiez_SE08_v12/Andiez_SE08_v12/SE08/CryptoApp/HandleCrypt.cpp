#include "HandleCrypt.h"

void HandleCrypt::handleEncryptDecrypt(HandleInfo* info)
{
	bool success = false;
	wstring name = (info->isEncrypt == Option::ENCRYPTION) ? L"Encryption" : L"Decryption";
	switch (info->algorithm)
	{
	case Algorithm::XOR_ENCRYPTION:
	case Algorithm::XOR_DECRYPTION:
	case Algorithm::RC4_ENCRYPTION:
	case Algorithm::RC4_DECRYPTION:
		success = handleCryptByte(info->hWnd, info->hEdit, info->algorithm);
		break;
	case Algorithm::AES_ENCRYPTION:
	case Algorithm::AES_DECRYPTION:
		success = handleCryptBlock(info->hWnd, info->hEdit, info->algorithm);
		break;
	default:
		MessageBox(info->hWnd, (L"Error: " + name + L" algorithm selected").c_str(), L"Error", MB_OK | MB_ICONERROR);
		break;
	}
	if (success) {
		handleUpdateLogs(info->hWnd, info->hEdit, (L"=> " + name + L" successful.").c_str());
	}
	else {
		handleUpdateLogs(info->hWnd, info->hEdit, (L"=> " + name + L" failed.").c_str());
	}
}

bool HandleCrypt::handleCryptByte(HWND hWnd, HWND hEdit, Algorithm option)
{
	BYTE* buffer = nullptr;
	DWORD bufferSize = 0;
	HANDLE hFileRead = nullptr;
	HANDLE hFileWrite = nullptr;

	BYTE newkey[MAX_PATH] = { 0 };
	size_t keySize = strlen(reinterpret_cast<const char*>(szKey));
	handleKey(szKey, keySize, newkey);

	DWORD fileSize;
	wstring newPath = { 0 };
	bool iscomplete = true;
	int progress = 0;
	DWORD bytesRead = 0;
	DWORD bytesWrite = 0;
	DWORD totalBytesRead = 0;
	DWORD totalBytesWrite = 0;

	BYTE digestIn[16];
	MD5_CTX contextIn;
	MD5Init(&contextIn);
	BYTE digestOut[16];
	MD5_CTX contextOut;
	MD5Init(&contextOut);

	SendMessage(GetDlgItem(hWnd, IDC_PROGRESS), PBM_SETPOS, 0, 0);
	newPath = removeLastPathComponent(szFileNameOut);
	if (createNestedDir(newPath) == false) {
		MessageBox(hWnd, L"Directory path error! Check the link again.", L"Error", MB_OK | MB_ICONERROR);
		return iscomplete = false;
	}

	hFileRead = CreateFileW(szFileNameIn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileRead == INVALID_HANDLE_VALUE) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_OPEN_FILE_IN].c_str());
		return iscomplete = false;
	}
	hFileWrite = CreateFileW(szFileNameOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileWrite == INVALID_HANDLE_VALUE) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_OPEN_FILE_OUT].c_str());
		CloseHandle(hFileRead);
		return iscomplete = false;
	}
	fileSize = GetFileSize(hFileRead, nullptr);
	bufferSize = (fileSize < BUFFER_SIZE) ? fileSize : BUFFER_SIZE;

	buffer = new BYTE[bufferSize];
	if (buffer == nullptr) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_ALLOCATE_MEMORY].c_str());
		CloseHandle(hFileRead);
		CloseHandle(hFileWrite);
		return iscomplete = false;
	}
	ULONGLONG start = GetTickCount64();
	while (totalBytesRead < fileSize) {
		if (!ReadFile(hFileRead, buffer, bufferSize, &bytesRead, NULL)) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_READ_FILE].c_str());
			iscomplete = false;
			break;
		}
		if (bytesRead == 0) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[ZERO_BUFFER_SIZE].c_str());
			break;
		}
		totalBytesRead += bytesRead;
		MD5Update(&contextIn, buffer, bytesRead);		//Hash MD5

		switch (option) {
		case Algorithm::XOR_ENCRYPTION:
		case Algorithm::XOR_DECRYPTION:
			XOR_Crypt(buffer, bytesRead, newkey, keySize);
			break;
		case Algorithm::RC4_ENCRYPTION:
		case Algorithm::RC4_DECRYPTION:
			RC4_Crypt(buffer, bytesRead, newkey, keySize);
			break;
		default:
			break;
		}
		if (!WriteFile(hFileWrite, buffer, bytesRead, &bytesWrite, NULL)) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_WRITE_FILE].c_str());
			iscomplete = false;
			break;
		}
		totalBytesWrite += bytesWrite;
		MD5Update(&contextOut, buffer, bytesWrite);		//Hash MD5
		progress = (int)((double)totalBytesWrite / fileSize * 100);
		SendMessage(GetDlgItem(hWnd, IDC_PROGRESS), PBM_SETPOS, progress, 0);
	}
	ULONGLONG end = GetTickCount64();
	ULONGLONG elapsed = (end - start) / 1000;

	MD5Final(&contextIn, digestIn);
	handleUpdateLogsHash(hWnd, hEdit, true, digestIn);
	MD5Final(&contextOut, digestOut);
	handleUpdateLogsHash(hWnd, hEdit, false, digestOut);

	if (iscomplete == true) {
		wstring elapsedTimeStr = L"- Execution time: " + to_wstring(elapsed) + L" seconds";
		handleUpdateLogs(hWnd, hEdit, elapsedTimeStr.c_str());
	}
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	if (hFileRead != nullptr || hFileWrite != nullptr) {
		CloseHandle(hFileRead);
		CloseHandle(hFileWrite);
	}
	return iscomplete;
}

bool HandleCrypt::handleCryptBlock(HWND hWnd, HWND hEdit, Algorithm option)
{
	HANDLE hFileRead = nullptr;
	HANDLE hFileWrite = nullptr;
	BYTE* buffer = nullptr;
	BYTE* bufferCrypt = nullptr;
	DWORD bufferSize = 0;
	DWORD fileSize = 0;
	wstring newPath;

	AES aes(AESKeyLength::AES_256);
	BYTE iv[16];
	BYTE newKey[32];
	aes.generateIV(szKey, iv);               // Key -> Hash MD5 -> iv
	aes.generateKey(szKey, newKey);          // Key -> Hash SHA256 -> newKey

	bool iscomplete = true;
	int progress = 0;
	size_t size = 0;
	DWORD bytesRead = 0;
	DWORD bytesWrite = 0;
	DWORD totalBytesRead = 0;
	DWORD totalBytesWrite = 0;

	BYTE digestIn[16];
	MD5_CTX contextIn;
	MD5Init(&contextIn);
	BYTE digestOut[16];
	MD5_CTX contextOut;
	MD5Init(&contextOut);

	SendMessage(GetDlgItem(hWnd, IDC_PROGRESS), PBM_SETPOS, 0, 0);
	newPath = removeLastPathComponent(szFileNameOut);
	if (createNestedDir(newPath) == false) {
		MessageBox(hWnd, L"Directory path error! Check the link again.", L"Error", MB_OK | MB_ICONERROR);
		return iscomplete = false;
	}
	hFileRead = CreateFileW(szFileNameIn, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileRead == INVALID_HANDLE_VALUE) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_OPEN_FILE_IN].c_str());
		return iscomplete = false;
	}
	hFileWrite = CreateFileW(szFileNameOut, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hFileWrite == INVALID_HANDLE_VALUE) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_OPEN_FILE_OUT].c_str());
		CloseHandle(hFileRead);
		return iscomplete = false;
	}
	fileSize = GetFileSize(hFileRead, nullptr);		//Get file size
	bufferSize = (fileSize < BUFFER_SIZE) ? (fileSize + 16) : BUFFER_SIZE;

	buffer = new BYTE[bufferSize];
	if (buffer == nullptr) {
		handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_ALLOCATE_MEMORY].c_str());
		CloseHandle(hFileRead);
		CloseHandle(hFileWrite);
		return iscomplete = false;
	}
	bufferCrypt = new BYTE[bufferSize];   // Create a new buffer for the encrypted data
	if (bufferCrypt == nullptr) {
		delete[] buffer;
		handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_ALLOCATE_MEMORY].c_str());
		return iscomplete = false;
	}
	memset(bufferCrypt, 0, bufferSize);         // Initialize the buffer with zeros
	ULONGLONG start = GetTickCount64();
	while (totalBytesRead < fileSize) {
		if (!ReadFile(hFileRead, buffer, bufferSize, &bytesRead, NULL)) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_READ_FILE].c_str());
			iscomplete = false;
			break;
		}
		if (bytesRead == 0) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[ZERO_BUFFER_SIZE].c_str());
			iscomplete = false;
			break;
		}
		totalBytesRead += bytesRead;
		MD5Update(&contextIn, buffer, bytesRead);     // Hash MD5

		if (option == AES_ENCRYPTION) {
			if (totalBytesRead == fileSize && aes.addPadding(buffer, bytesRead) == false) {
				handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_ADD_PADDING].c_str());
				iscomplete = false;
				break;
			}
			aes.encryptCBC(buffer, bytesRead, newKey, iv, bufferCrypt);
			size = fileSize;
		}
		else {
			aes.decryptCBC(buffer, bytesRead, newKey, iv, bufferCrypt);
			if (totalBytesRead == fileSize && aes.removePadding(bufferCrypt, bytesRead) == false) {
				handleUpdateLogs(hWnd, hEdit, MessagesError[ERROR_REMOVE_PADDING].c_str());
				iscomplete = false;
				break;
			}
			size = fileSize - 16;
		}

		if (!WriteFile(hFileWrite, bufferCrypt, bytesRead, &bytesWrite, NULL)) {
			handleUpdateLogs(hWnd, hEdit, MessagesError[FAILED_TO_WRITE_FILE].c_str());
			iscomplete = false;
			break;
		}
		totalBytesWrite += bytesWrite;
		MD5Update(&contextOut, bufferCrypt, bytesWrite);  // Hash MD5

		progress = (int)((double)totalBytesWrite / size * 100);		//Update progress bar
		SendMessage(GetDlgItem(hWnd, IDC_PROGRESS), PBM_SETPOS, progress, 0);
	}
	ULONGLONG end = GetTickCount64();
	ULONGLONG elapsed = (end - start) / 1000;

	MD5Final(&contextIn, digestIn);
	MD5Final(&contextOut, digestOut);
	handleUpdateLogsHash(hWnd, hEdit, true, digestIn);
	handleUpdateLogsHash(hWnd, hEdit, false, digestOut);

	if (iscomplete == true) {
		wstring elapsedTimeStr = L"- Execution time: " + to_wstring(elapsed) + L" seconds";
		handleUpdateLogs(hWnd, hEdit, elapsedTimeStr.c_str());
	}
	if (buffer != nullptr || bufferCrypt != nullptr) {
		delete[] buffer;
		buffer = nullptr;
		delete[] bufferCrypt;
		bufferCrypt = nullptr;
	}
	if (hFileRead != nullptr || hFileWrite != nullptr) {
		CloseHandle(hFileRead);
		CloseHandle(hFileWrite);
	}
	return iscomplete;
}

void HandleCrypt::handleKey(BYTE* key, size_t length, BYTE* newKey)
{
	memcpy(newKey, key, length);
	for (size_t i = 0; i < length; i++) {
		newKey[i] ^= length;
	}
}

void HandleCrypt::handleUpdateLogs(HWND hWnd, HWND hEdit, const WCHAR * message)
{
	SetLastError(0);
	int len = GetWindowTextLength(hEdit);
	if (len > 0) {
		SendMessage(hEdit, EM_SETSEL, len, len);
	}
	DWORD errorCode = GetLastError();
	if (errorCode != 0) {
		LPVOID errorMessage = nullptr;
		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM, NULL, errorCode, 0, (LPWSTR)& errorMessage, 0, NULL);
		MessageBox(hWnd, (LPCWSTR)errorMessage, L"Error", MB_OK);
		LocalFree(errorMessage);
		return;
	}
	wstring newLine = wstring(message) + L"\n";
	SendMessage(hEdit, EM_REPLACESEL, FALSE, (LPARAM)newLine.c_str());
}

void HandleCrypt::handleUpdateLogsHash(HWND hWnd, HWND hEdit, bool isInput, BYTE* digest)
{
	wstring logMessage = isInput ? L"- Input hash: " : L"- Output hash: ";
	for (int i = 0; i < MD5_DIGEST_LENGTH; i++) {
		wchar_t hexChar[3];							// Save 2 hex characters and null-terminator character
		swprintf_s(hexChar, L"%02X", digest[i]);		// Convert bytes to hex
		logMessage += hexChar;						// Add hex characters to the log string
	}
	handleUpdateLogs(hWnd, hEdit, logMessage.c_str());
}

bool HandleCrypt::createNestedDir(const wstring & path)
{
	wstring parentPath = { 0 };
	if (path.size() > MAX_PATH)
	{
		return false;
	}
	if (PathFileExistsW(path.c_str())) {
		return true;
	}
	else {
		size_t lastSeparatorPos = path.find_last_of(L"\\");
		if (lastSeparatorPos != wstring::npos && lastSeparatorPos > 1) {
			parentPath = path.substr(0, lastSeparatorPos);
			if (createNestedDir(parentPath)) {
				return CreateDirectoryW(path.c_str(), NULL);
			}
		}
		return false;
	}
}

wstring HandleCrypt::removeLastPathComponent(const wstring & path)
{
	size_t lastSeparatorPos = path.find_last_of(L"\\");
	if (lastSeparatorPos != wstring::npos && lastSeparatorPos > 1) {
		return path.substr(0, lastSeparatorPos);
	}
	return path;
}
