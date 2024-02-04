#include "resource.h"
#include "CryptoApp.h"
#include "HandleCrypt.h"
#include "Initialize.h"
#include "HandleAutoRun.h"
#include "Service.h"

HINSTANCE hInst;
CryptoApp myApp;

WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
WCHAR szWindowClass[MAX_LOADSTRING];            // The main window class name

// Forward declarations of functions included in this code module:
ATOM MyRegisterClass(HINSTANCE hInstance);
BOOL InitInstance(HINSTANCE, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);

// Handle DialogBox Task Scheduler
LRESULT CALLBACK DialogProcTask(HWND, UINT, WPARAM, LPARAM);
ATOM registerDialogTask(HINSTANCE);
HandleTask task;

// Handle DialogBox Service
LRESULT CALLBACK DialogProcService(HWND, UINT, WPARAM, LPARAM);
ATOM registerDialogService(HINSTANCE);
ServiceError error;

DWORD pathLength = GetModuleFileName(NULL, pathApp, MAX_PATH);

int APIENTRY wWinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPWSTR lpCmdLine, _In_ int nCmdShow)
{
	UNREFERENCED_PARAMETER(hPrevInstance);
	if (wcscmp(lpCmdLine, L"--srv") == 0)
	{
		SERVICE_TABLE_ENTRY ServiceTable[] =
		{
			{(LPWSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
			{NULL, NULL}
		};

		if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
		{
			return GetLastError();
		}
		return 0;
	}
	else
	{
		LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
		LoadStringW(hInstance, IDC_CRYPTOAPP, szWindowClass, MAX_LOADSTRING);
		MyRegisterClass(hInstance);

		registerDialogTask(hInst);
		registerDialogService(hInst);

		if (!InitInstance(hInstance, nCmdShow))
		{
			return FALSE;
		}
		HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CRYPTOAPP));
		MSG msg;
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
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW);
	wcex.lpszMenuName = MAKEINTRESOURCEW(IDC_CRYPTOAPP);
	wcex.lpszClassName = szWindowClass;
	wcex.hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL));
	return RegisterClassExW(&wcex);
}

BOOL InitInstance(HINSTANCE hInstance, int nCmdShow)
{
	hInst = hInstance;
	HWND hWnd = CreateWindowW(
		szWindowClass, szTitle,
		WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
		CW_USEDEFAULT, CW_USEDEFAULT, 450, 760,
		nullptr, nullptr, hInstance, nullptr);
	if (!hWnd)
	{
		return FALSE;
	}
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);
	return TRUE;
}


LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	myApp.hInst = hInst;
	myApp.hWnd = hWnd;
	switch (message)
	{
	case WM_QUERYENDSESSION:
		// Handle shutdown
		break;
	case WM_CREATE:
	{
		myApp.createMenuOption();
		myApp.createRunCloseButton();
		myApp.createGroupMethod();
		myApp.createGroupAlgorithm();
		myApp.createInputPath();
		myApp.createOutputPath();
		myApp.createEnterKey();
		myApp.createProgressBar();
		myApp.createShowLogs();
	}
	break;
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == BN_CLICKED) {
			myApp.handleButton(LOWORD(wParam));
		}
		if (HIWORD(wParam) == EN_CHANGE) {
			myApp.handleTextBox(LOWORD(wParam));
		}
		if (LOWORD(wParam) == IDM_STARTUP_FOLDER) {
			if (pathLength > 0) {
				handleStartup(L"\\CryptoApp.lnk", pathApp, L"This is my application!");
				MessageBox(hWnd, L"You've configured an application for automatic startup by placing it in the Startup Folder.", L"Notification", MB_OK);
			}
			else {
				MessageBox(hWnd, L"Unable to get the path to the application.", L"Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		if (LOWORD(wParam) == IDM_REGISTRY_EDITOR) {
			if (pathLength > 0) {
				handleRegistry(L"CryptoApp", pathApp, HKCU);
				MessageBox(hWnd, L"You've configured an application for automatic startup by placing it in the Registry Editor.", L"Notification", MB_OK);
			}
			else {
				MessageBox(hWnd, L"Unable to get the path to the application.", L"Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		if (LOWORD(wParam) == IDM_TASK_SCHEDULER) {
			myApp.createDialogBoxTask();
			if (pathLength > 0) {
				task.wstrExecutablePath = pathApp;
			}
			else {
				MessageBox(hWnd, L"Unable to get the path to the application.", L"Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		if (LOWORD(wParam) == IDM_SERVICES) {
			myApp.createDialogBoxService();
			if (pathLength < 0) {
				MessageBox(hWnd, L"Unable to get the path to the application.", L"Error", MB_OK | MB_ICONERROR);
			}
			break;
		}
		if (LOWORD(wParam) == IDM_ABOUT) {
			DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
		}
		if (LOWORD(wParam) == IDM_EXIT) {
			DestroyWindow(hWnd);
		}
	}
	break;
	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);
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

LRESULT CALLBACK DialogProcTask(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	myApp.hDialogTask = hDialog;
	switch (message)
	{
	case WM_CREATE:
	{
		myApp.createStaticBoxTask();
		myApp.createComboBoxTask();
		myApp.createOkButtonTask();
		myApp.createStaticTask();
	}
	break;
	case WM_COMMAND:
	{
		if (HIWORD(wParam) == CBN_SELCHANGE || HIWORD(wParam) == CBN_EDITCHANGE) {
			myApp.handleComboBox(LOWORD(wParam));
			task.timeRepeat = myApp.timeRepeat;
		}
		if (HIWORD(wParam) == EN_CHANGE) {
			myApp.handleComboBox(LOWORD(wParam));
			task.timeRepeat = myApp.timeRepeat;
		}
		if (LOWORD(wParam) == OK) {
			if (handleTaskSchd(task) == true) {
				MessageBox(hDialog, L"Successfully set up tack scheduler", L"Notification", MB_OK);
			}
			else {
				MessageBox(hDialog, MessagesErrorTaskSchd[task.error].c_str(), L"Error", MB_OK | MB_ICONERROR);
			}
			DestroyWindow(hDialog);
		}
	}
	break;
	case WM_INITDIALOG:
		break;
	default:
		return DefWindowProc(hDialog, message, wParam, lParam);
	}
	return 0;
}

ATOM registerDialogTask(HINSTANCE hInstance)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)DialogProcTask;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = TEXT("DialogClass");
	return RegisterClassEx(&wc);
}

LRESULT CALLBACK DialogProcService(HWND hDialog, UINT message, WPARAM wParam, LPARAM lParam)
{
	myApp.hDialogService = hDialog;
	switch (message)
	{
	case WM_CREATE:
	{
		myApp.createButtonService();
		myApp.createStaticService();
	}
	break;
	case WM_COMMAND:
	{
		if (LOWORD(wParam) == INSTALL_SV) {
			if (SvcInstall(error) == true) {
				MessageBox(hDialog, L"Successfully install service", L"Notification", MB_OK);
			}
			else {
				MessageBox(hDialog, MessegerErrorService[error].c_str(), L"Error", MB_OK | MB_ICONERROR);
			}
			DestroyWindow(hDialog);
		}
		if (LOWORD(wParam) == UNINSTALL_SV) {
			if (SvcUninstall(error) == true) {
				MessageBox(hDialog, L"Successfully uninstall service", L"Notification", MB_OK);
			}
			else {
				MessageBox(hDialog, MessegerErrorService[error].c_str(), L"Error", MB_OK | MB_ICONERROR);
			}
			DestroyWindow(hDialog);
		}
	}
	break;
	case WM_INITDIALOG:
		break;
	default:
		return DefWindowProc(hDialog, message, wParam, lParam);
	}
	return 0;
}

ATOM registerDialogService(HINSTANCE hInstance)
{
	WNDCLASSEX wc = { 0 };
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.lpfnWndProc = (WNDPROC)DialogProcService;
	wc.hInstance = hInstance;
	wc.hbrBackground = GetSysColorBrush(COLOR_3DFACE);
	wc.lpszClassName = TEXT("DialogService");
	return RegisterClassEx(&wc);
}
