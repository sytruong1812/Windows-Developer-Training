#include "CryptoApp.h"

using namespace std;

bool isRunning = false;
HFONT hf = nullptr;
HANDLE hThread = nullptr;
DWORD WINAPI myHandleCrypt(LPVOID lpParam);

CryptoApp::CryptoApp()
{
	this->hInst = nullptr;
	this->hWnd = nullptr;
	this->hEdit = nullptr;
	this->hPathIn = nullptr;
	this->hPathKey = nullptr;
	this->hPathOut = nullptr;
}

Algorithm CryptoApp::getSelectedAlgorithm(Option myOption)
{
	Algorithm algorithm = Algorithm::XOR_ENCRYPTION;
	if (IsDlgButtonChecked(hWnd, XOR_ALGORITHM) == BST_CHECKED) {
		algorithm = (myOption == Option::ENCRYPTION) ? Algorithm::XOR_ENCRYPTION : Algorithm::XOR_DECRYPTION;
	}
	else if (IsDlgButtonChecked(hWnd, RC4_ALGORITHM) == BST_CHECKED) {
		algorithm = (myOption == Option::ENCRYPTION) ? Algorithm::RC4_ENCRYPTION : Algorithm::RC4_DECRYPTION;
	}
	else if (IsDlgButtonChecked(hWnd, AES_ALGORITHM) == BST_CHECKED) {
		algorithm = (myOption == Option::ENCRYPTION) ? Algorithm::AES_ENCRYPTION : Algorithm::AES_DECRYPTION;
	}
	return algorithm;
}

DWORD WINAPI myHandleCrypt(LPVOID lpParam)
{
	HandleInfo* info = reinterpret_cast<HandleInfo*>(lpParam);
	isRunning = true;
	HandleCrypt myHandle;
	myHandle.handleEncryptDecrypt(info);
	isRunning = false;
	return 0;
}
void CryptoApp::handleButton(const int ID)
{
	info.hWnd = hWnd;
	info.hEdit = hEdit;
	info.algorithm = getSelectedAlgorithm(info.isEncrypt);
	switch (ID)
	{
	case RUN:
	{
		int keyLen = GetWindowTextLength(hPathKey) + 1;
		if (keyLen < MIN_KEYLENGTH) {
			MessageBox(hWnd, L"Make sure the key is long enough to not be broken [5 <= Key length <= 260]", L"Warning", MB_OK | MB_ICONERROR);
			break;
		}
		if (isRunning) {
			MessageBox(hWnd, L"A thread is already running. Please wait for it to finish.", L"Warning", MB_OK | MB_ICONWARNING);
			return;
		}
		hThread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)myHandleCrypt, &info, 0, NULL);
		if (hThread == NULL) {
			MessageBox(hWnd, L"Error: Failed to create thread. Unable to proceed with encryption!", L"Error", MB_OK | MB_ICONERROR);
		}
		else {
			CloseHandle(hThread);
		}
	}
	break;
	case ENCRYPT:
		info.isEncrypt = Option::ENCRYPTION;
		break;
	case DECRYPT:
		info.isEncrypt = Option::DECRYPTION;
		break;
	case BROWSE_FILE_INPUT:
		handleBrowseFiles(BROWSE_FILE_INPUT);
		break;
	case BROWSE_FILE_OUTPUT:
		handleBrowseFiles(BROWSE_FILE_OUTPUT);
		break;
	case CLOSE:
		DestroyWindow(hWnd);
		break;
	default:
		break;
	}
}

void CryptoApp::handleTextBox(const int ID)
{
	switch (ID) {
	case EDIT_FILENAME_INPUT:
	case EDIT_FILENAME_OUTPUT:
	{
		HWND hPath = (ID == EDIT_FILENAME_INPUT) ? hPathIn : hPathOut;
		wchar_t* sizeFileName = (ID == EDIT_FILENAME_INPUT) ? szFileNameIn : szFileNameOut;
		int length = GetWindowTextLength(hPath) + 1;
		if (length <= MAX_PATH) {
			wchar_t* path = new wchar_t[MAX_PATH];
			if (path == nullptr) {
				MessageBox(hWnd, MessagesError[FAILED_TO_ALLOCATE_MEMORY].c_str(), L"Error", MB_OK | MB_ICONERROR);
				return;
			}
			GetWindowTextW(hPath, path, MAX_PATH);
			wcscpy_s(sizeFileName, MAX_PATH, path);
			delete[] path;
		}
		else {
			MessageBox(hWnd, L"Error: Path length > MAX_PATH (260).", L"Error", MB_OK | MB_ICONERROR);
		}
	}
	break;
	case EDIT_KEY:
	{
		int length = GetWindowTextLength(hPathKey) + 1;
		if (length <= MAX_KEYLENGTH) {
			BYTE* temp = new BYTE[MAX_KEYLENGTH];
			if (temp == nullptr) {
				MessageBox(hWnd, MessagesError[FAILED_TO_ALLOCATE_MEMORY].c_str(), L"Error", MB_OK | MB_ICONERROR);
				return;
			}
			GetWindowTextA(hPathKey, reinterpret_cast<LPSTR>(temp), MAX_KEYLENGTH);
			memcpy(szKey, temp, length);
			delete[] temp;
		}
		else {
			MessageBox(hWnd, L"Error: Key length > MAX_KEYLENGTH (260).", L"Error", MB_OK | MB_ICONERROR);
		}
	}
	break;
	default:
		break;
	}
}

void CryptoApp::handleBrowseFiles(const int ID)
{
	wchar_t envPath[MAX_PATH];
	ExpandEnvironmentStringsW(L"%USERPROFILE%", envPath, MAX_PATH);

	OPENFILENAME ofn;
	ZeroMemory(&ofn, sizeof(ofn));
	ofn.lStructSize = sizeof(ofn);
	ofn.hwndOwner = hWnd;
	ofn.lpstrFilter = L"All Files\0*.*\0";
	ofn.lpstrFile = szFileName;
	ofn.nMaxFile = MAX_PATH;
	ofn.lpstrInitialDir = envPath;
	if (ID == BROWSE_FILE_INPUT) {
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;
		GetOpenFileName(&ofn);
		wcscpy_s(szFileNameIn, MAX_PATH, szFileName);
		SetWindowText(hPathIn, szFileNameIn);
	}
	else if (ID == BROWSE_FILE_OUTPUT) {
		ofn.Flags = OFN_CREATEPROMPT | OFN_OVERWRITEPROMPT;
		GetSaveFileName(&ofn);
		wcscpy_s(szFileNameOut, MAX_PATH, szFileName);
		SetWindowText(hPathOut, szFileNameOut);
	}
}

void CryptoApp::createRunCloseButton()
{
	CreateWindowW(
		L"BUTTON", L"Run",
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		140, 655, 60, 30,
		hWnd, (HMENU)RUN, hInst, NULL);
	CreateWindowW(
		L"BUTTON", L"Close",
		WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		215, 655, 60, 30,
		hWnd, (HMENU)CLOSE, hInst, NULL);
}

void CryptoApp::createGroupMethod()
{
	CreateWindowW(
		L"BUTTON", L"Select method",
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		10, 10, 110, 90,
		hWnd, (HMENU)NULL, hInst, NULL);
	HWND hEncrypt = CreateWindowW(
		L"BUTTON", L"Encrypt",
		WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
		20, 30, 85, 30,
		hWnd, (HMENU)ENCRYPT, hInst, NULL);
	HWND hDecrypt = CreateWindowW(
		L"BUTTON", L"Decrypt",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		20, 60, 85, 30,
		hWnd, (HMENU)DECRYPT, hInst, NULL);
	SendMessage(hEncrypt, BM_SETCHECK, BST_CHECKED, 0);
}

void CryptoApp::createInputPath()
{
	CreateWindowW(
		L"STATIC", L"Select file to encrypt or decrypt",
		WS_CHILD | WS_VISIBLE,
		10, 110, 205, 20,
		hWnd, (HMENU)NULL, hInst, NULL);
	hPathIn = CreateWindowW(
		L"EDIT", L"",
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		10, 135, 340, 20,
		hWnd, (HMENU)EDIT_FILENAME_INPUT, hInst, NULL);
	CreateWindowW(
		L"BUTTON", L"Browse",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		360, 135, 60, 20,
		hWnd, (HMENU)BROWSE_FILE_INPUT, hInst, NULL);
	hf = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	SendMessage(hPathIn, WM_SETFONT, (WPARAM)hf, 0);
}

void CryptoApp::createGroupAlgorithm()
{
	CreateWindowW(
		L"BUTTON", L"Select an encryption algorithm",
		WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
		10, 170, 215, 120,
		hWnd, (HMENU)NULL, hInst, NULL);
	HWND hXor = CreateWindowW(
		L"BUTTON", L"XOR",
		WS_VISIBLE | WS_CHILD | WS_GROUP | BS_AUTORADIOBUTTON,
		20, 195, 60, 30,
		hWnd, (HMENU)XOR_ALGORITHM, hInst, NULL);
	HWND hRc4 = CreateWindowW(
		L"BUTTON", L"RC4",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		20, 225, 60, 30,
		hWnd, (HMENU)RC4_ALGORITHM, hInst, NULL);
	HWND hAes = CreateWindowW(
		L"BUTTON", L"AES256 + CBC + PKCS7",
		WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
		20, 255, 180, 30,
		hWnd, (HMENU)AES_ALGORITHM, hInst, NULL);
	SendMessage(hXor, BM_SETCHECK, BST_CHECKED, 0);
}

void CryptoApp::createOutputPath()
{
	CreateWindowW(
		L"STATIC", L"Select output path",
		WS_CHILD | WS_VISIBLE,
		10, 360, 120, 20,
		hWnd, (HMENU)NULL, hInst, NULL);
	hPathOut = CreateWindowW(
		L"EDIT", L"",
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		10, 385, 340, 20,
		hWnd, (HMENU)EDIT_FILENAME_OUTPUT, hInst, NULL);
	CreateWindowW(
		L"BUTTON", L"Browse",
		WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
		360, 385, 60, 20,
		hWnd, (HMENU)BROWSE_FILE_OUTPUT, hInst, NULL);
	hf = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	SendMessage(hPathOut, WM_SETFONT, (WPARAM)hf, 0);
}

void CryptoApp::createEnterKey()
{
	CreateWindowW(
		L"STATIC", L"Encryption key",
		WS_CHILD | WS_VISIBLE,
		10, 300, 100, 20,
		hWnd, (HMENU)NULL, hInst, NULL);
	hPathKey = CreateWindowW(
		L"EDIT", L"",
		WS_TABSTOP | WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
		10, 325, 340, 20,
		hWnd, (HMENU)EDIT_KEY, hInst, NULL);
	hf = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	SendMessage(hPathKey, WM_SETFONT, (WPARAM)hf, 0);
}

void CryptoApp::createProgressBar()
{
	CreateWindowW(
		L"STATIC", L"Status",
		WS_CHILD | WS_VISIBLE,
		10, 420, 45, 20,
		hWnd, (HMENU)NULL, hInst, NULL);
	HWND hProgress = CreateWindowW(
		PROGRESS_CLASS, NULL,
		WS_CHILD | WS_VISIBLE,
		10, 445, 340, 20,
		hWnd, (HMENU)IDC_PROGRESS, hInst, NULL);
	if (hProgress != NULL) {
		SendMessage(hProgress, PBM_SETRANGE, 0, MAKELPARAM(0, 100));
	}
}

void CryptoApp::createShowLogs()
{
	CreateWindowW(
		L"STATIC", L"Logs",
		WS_CHILD | WS_VISIBLE,
		10, 480, 35, 20,
		hWnd, (HMENU)NULL, hInst, NULL);
	hEdit = CreateWindowExW(WS_EX_CLIENTEDGE, L"EDIT", NULL,
		WS_CHILD | WS_VISIBLE | WS_VSCROLL | WS_HSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL,
		10, 505, 405, 135, hWnd, NULL, hInst, NULL);
	hf = CreateFont(16, 0, 0, 0, FW_BOLD, 0, 0, 0, ANSI_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, L"Arial");
	SendMessage(hEdit, WM_SETFONT, (WPARAM)hf, 0);
}