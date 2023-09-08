#include "framework.h"
#include "CryptoApp.h"

#define MAX_LOADSTRING 100

// Global Variables:
HINSTANCE hInst;                                // current instance
WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

// Forward declarations of functions included in this code module:
ATOM                MyRegisterClass(HINSTANCE hInstance);
BOOL                InitInstance(HINSTANCE, int);
LRESULT CALLBACK    WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);

HINSTANCE g_hinst;						//Process bar
TCHAR path1[] = L"C:\\Users\\SY TRUONG\\Documents\\MicronSoftware\\Resource\\file1GB_0.txt";
TCHAR path2[] = L"C:\\Users\\SY TRUONG\\Documents\\MicronSoftware\\Resource\\file1GB_OUT.txt";
BOOL ParseALargeFile(HWND hwndParent, LPTSTR lpszFileName1, LPTSTR lpszFileName2);


int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	UNREFERENCED_PARAMETER(lpCmdLine);

	// TODO: Place code here.
	InitCommonControls();

	// Initialize global strings
	LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadStringW(hInstance, IDC_CRYPTOAPP, szWindowClass, MAX_LOADSTRING);
	MyRegisterClass(hInstance);

	// Perform application initialization:
	if (!InitInstance(hInstance, nCmdShow))
	{
		return FALSE;
	}

	HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CRYPTOAPP));

	MSG msg;

	// Main message loop:
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return (int)msg.wParam;
}

ATOM MyRegisterClass(HINSTANCE hInstance)
{
	WNDCLASSEXW wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpfnWndProc = WndProc;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.hInstance = hInstance;
	wcex.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRYPTOAPP));
	wcex.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CRYPTOAPP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

/*
FUNCTION: InitInstance(HINSTANCE, int)
PURPOSE: Saves instance handle and creates main window
COMMENTS: In this function, we save the instance handle in a
	global variable and create and display the main program window.
*/
BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance; // Store instance handle in our global variable

	HWND hWnd = CreateWindowW(szWindowClass, szTitle, WS_OVERLAPPEDWINDOW | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 400, 600, nullptr, nullptr, hInstance, nullptr);

	if (!hWnd)
	{
		return FALSE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;
}

/*
FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
PURPOSE: Processes messages for the main window.
	WM_CREATE	-
	WM_COMMAND  - process the application menu
	WM_PAINT    - Paint the main window
	WM_DESTROY  - post a quit message and return
*/
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_CREATE:
	{
		CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Enter 1",      // Button text 
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			10,         // x position 
			10,         // y position 
			60,         // Button width
			60,         // Button height
			hWnd,       // Parent window
			(HMENU)BUTTON_1,   // Button ID
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);      // Pointer not needed.

		CreateWindowW(
			L"BUTTON",  // Predefined class; Unicode assumed 
			L"Enter 2",      // Button text 
			WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,  // Styles 
			80,         // x position 
			10,         // y position 
			60,         // Button width
			60,         // Button height
			hWnd,       // Parent window
			(HMENU)BUTTON_2,   // Button ID
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);      // Pointer not needed.

		CreateWindowW(
			L"BUTTON",
			L"Select method",
			WS_VISIBLE | WS_CHILD | BS_GROUPBOX,
			100,         // x position 
			100,         // y position 
			110,         // Button width
			100,         // Button height
			hWnd,
			(HMENU)0,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		CreateWindowW(
			L"BUTTON",
			L"Encrypt",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			110,
			130,
			85,
			30,
			hWnd,
			(HMENU)ENCRYPT,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		CreateWindowW(
			L"BUTTON",
			L"Decrypt",
			WS_VISIBLE | WS_CHILD | BS_AUTORADIOBUTTON,
			110,
			160,
			85,
			30,
			hWnd,
			(HMENU)DECRYPT,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);

		CreateWindowW(
			L"BUTTON",
			L"Browse",
			WS_TABSTOP | WS_VISIBLE | WS_CHILD | BS_DEFPUSHBUTTON,
			200,
			300,
			100,
			30,
			hWnd,
			(HMENU)BROWSE,
			(HINSTANCE)GetWindowLongPtr(hWnd, GWLP_HINSTANCE),
			NULL);
		break;
	}
	case WM_COMMAND:
	{
		int wmId = LOWORD(wParam);

		OPENFILENAME ofn;
		TCHAR szFileName[MAX_PATH] = { 0 };		//Save path file
		ZeroMemory(&ofn, sizeof(ofn));
		ofn.lStructSize = sizeof(ofn);
		ofn.hwndOwner = hWnd;
		ofn.lpstrFilter = L"All Files\0*.*\0";
		ofn.lpstrFile = szFileName;
		ofn.nMaxFile = MAX_PATH;
		ofn.lpstrInitialDir = L"C:\\Users\\SY TRUONG\\Documents";
		ofn.Flags = OFN_EXPLORER | OFN_FILEMUSTEXIST;

		// Parse the menu selections:
		switch (wmId)
		{
		case IDM_ABOUT:
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
			break;
		case IDM_EXIT:
			DestroyWindow(hWnd);
			break;
		case BUTTON_1:
			//MessageBox(hWnd, L"ReadFile", L"Notification", MB_OK);
			ParseALargeFile(hWnd, path1, path2);
			break;
		case BUTTON_2:
			MessageBox(hWnd, L"Hello", L"Notification", MB_OK);
			break;
		case ENCRYPT:
			MessageBox(hWnd, L"Encrypt", L"Notification", MB_OK);
			break;
		case DECRYPT:
			MessageBox(hWnd, L"Decrypt", L"Notification", MB_OK);
			break;
		case BROWSE:
			if (GetOpenFileName(&ofn)) {
				MessageBox(hWnd, ofn.lpstrFile, L"Selected File", MB_OK);
			}
			break;
		default:
			return DefWindowProc(hWnd, message, wParam, lParam);
		}
	}
	break;
	case  WM_TIMER:
	{
		break;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code that uses hdc here...
		EndPaint(hWnd, &ps);
	}
	break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return 0;
}

BOOL ParseALargeFile(HWND hwndParent, LPTSTR lpszFileName1, LPTSTR lpszFileName2)
{
	RECT rcClient;  // Client area of parent window.
	int cyVScroll;  // Height of scroll bar arrow.
	HWND hwndPB;    // Handle of progress bar.
	HANDLE hFileRead;   // Handle of file.
	HANDLE hFileWrite;
	DWORD byteRead;       // Size of file and count of bytes read.
	DWORD byteWrite;
	//LPCH pch;       // Address of data read from file.
	//LPCH pchTmp;    // Temporary pointer.
	BOOL* buffer = nullptr;

	// Ensure that the common control DLL is loaded, and create a progress bar 
	// along the bottom of the client area of the parent window. 
	//
	// Base the height of the progress bar on the height of a scroll bar arrow.

	GetClientRect(hwndParent, &rcClient);

	cyVScroll = GetSystemMetrics(SM_CYVSCROLL);

	hwndPB = CreateWindowEx(0, PROGRESS_CLASS, (LPTSTR)NULL,
		WS_CHILD | WS_VISIBLE | PBS_SMOOTH, rcClient.left,
		rcClient.bottom - cyVScroll,
		rcClient.right, cyVScroll,
		hwndParent, (HMENU)0, g_hinst, NULL);


	// Open the file for reading, and retrieve the size of the file. 

	hFileRead = CreateFileW(lpszFileName1, GENERIC_READ, FILE_SHARE_READ, (LPSECURITY_ATTRIBUTES)NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hFileRead == (HANDLE)INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	hFileWrite = CreateFileW(lpszFileName2, GENERIC_WRITE, FILE_SHARE_WRITE, (LPSECURITY_ATTRIBUTES)NULL, CREATE_NEW, FILE_ATTRIBUTE_NORMAL, (HANDLE)NULL);
	if (hFileWrite == (HANDLE)INVALID_HANDLE_VALUE) {
		return FALSE;
	}

	byteRead = GetFileSize(hFileRead, (LPDWORD)NULL);

	// Set the range and increment of the progress bar. 

	SendMessage(hwndPB, PBM_SETRANGE, 0, MAKELPARAM(0, byteRead / 2048));
	SendMessage(hwndPB, PBM_SETSTEP, (WPARAM)1, 0);

	// Parse the file. 
	// 
	// pch = (LPCH)LocalAlloc(LPTR, sizeof(char) * 2048);
	// pchTmp = pch;

	buffer = new BOOL[2048];

	do {
		if (!ReadFile(hFileRead, buffer, 2048, &byteRead, NULL)) {
			MessageBox(NULL, L"Read file error!", L"Error", MB_OK);
			return FALSE;
		}

		if (!WriteFile(hFileWrite, buffer, byteRead, &byteWrite, NULL)) {
			MessageBox(NULL, L"Write file error!", L"Error", MB_OK);
			return FALSE;
		}
		SendMessage(hwndPB, PBM_STEPIT, 0, 0);
	} while (byteRead);

	CloseHandle((HANDLE)hFileRead);
	CloseHandle((HANDLE)hFileWrite);
	if (buffer != nullptr) {
		delete[] buffer;
		buffer = nullptr;
	}
	UpdateWindow(hwndPB);
	DestroyWindow(hwndPB);
	return TRUE;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (message)
	{
	case WM_INITDIALOG:
		return (INT_PTR)TRUE;

	case WM_COMMAND:
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hDlg, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}
		break;
	}
	return (INT_PTR)FALSE;
}

