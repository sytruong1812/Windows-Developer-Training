#include <string>
#include <tchar.h>
#include <windows.h>
#include <winsvc.h>
#include "Service.h"

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;
HANDLE				  g_ProcessHandle = NULL;

VOID InitializeServiceStatus(DWORD controlAccept, DWORD currentState, DWORD exitCode, DWORD checkPoint)
{
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwControlsAccepted = controlAccept;
	g_ServiceStatus.dwCurrentState = currentState;
	g_ServiceStatus.dwWin32ExitCode = exitCode;
	g_ServiceStatus.dwCheckPoint = checkPoint;
	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		writeToLogFile(L"InitializeServiceStatus: SetServiceStatus returned error");
	}
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:
		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING) {
			writeToLogFile(L"ServiceCtrlHandler: dwCurrentState != SERVICE_RUNNING");
			break;
		}
		if (g_ProcessHandle != NULL)
		{
			TerminateProcess(g_ProcessHandle, 0);
			CloseHandle(g_ProcessHandle);
			g_ProcessHandle = NULL;
		}
		InitializeServiceStatus(0, SERVICE_STOP_PENDING, 0, 4);
		SetEvent(g_ServiceStopEvent);
		writeToLogFile(L"ServiceCtrlHandler: Stop Service success.");
		break;
	default:
		break;
	}
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	DWORD Status = E_FAIL;
	DWORD Errors = 0;
	g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);	// Register our service control handler with the SCM
	if (g_StatusHandle == NULL)
	{
		writeToLogFile(L"ServiceMain: RegisterServiceCtrlHandler false");
		return;
	}
	writeToLogFile(L"ServiceMain: RegisterServiceCtrlHandler success.");
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));		// Tell the service controller we are starting
	InitializeServiceStatus(0, SERVICE_START_PENDING, 0, 0);
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);		// Create a service stop event to wait on later
	if (g_ServiceStopEvent == NULL)
	{
		InitializeServiceStatus(0, SERVICE_STOPPED, Errors, 1);
		return;
	}
	InitializeServiceStatus(SERVICE_ACCEPT_STOP, SERVICE_RUNNING, 0, 0);	// Tell the service controller we are started
	/*========================Start CryptoApp===========================*/
	wstring cmd = getPathFile(true);
	wstring cmdDir = getPathFile(false);
	if (!startProcessAsCurrentUser((LPTSTR)cmd.c_str(), cmdDir.c_str(), TRUE)) {
		writeToLogFile(L"ServiceMain: StartProcessAsCurrentUser returned error");
	}
	/*==================================================================*/
	CloseHandle(g_ServiceStopEvent);
	InitializeServiceStatus(0, SERVICE_STOPPED, 0, 3);	// Tell the service controller we are stopped
	writeToLogFile(L"ServiceMain: StartProcessAsCurrentUser success.");
}

BOOL getSessionUserToken(HANDLE& hUserToken)
{
	DWORD sessionCount = 0;		//Save session
	WTS_SESSION_INFO* pSessionInfo = nullptr;
	DWORD sessionID = 0xFFFFFFFF;
	//Get information about sessions and number of sessions.
	if (WTSEnumerateSessions(WTS_CURRENT_SERVER_HANDLE, 0, 1, &pSessionInfo, &sessionCount))
	{
		for (DWORD i = 0; i < sessionCount; ++i)
		{
			if (pSessionInfo[i].State == WTSActive)	//Check whether the session is Active or not?
			{
				sessionID = pSessionInfo[i].SessionId;
				break;
			}
		}
		WTSFreeMemory(pSessionInfo);
	}
	if (sessionID == 0xFFFFFFFF) {	//If no session is found in "Active" state, then get the Session ID of the active console session.
		sessionID = WTSGetActiveConsoleSessionId();
	}
	if (WTSQueryUserToken(sessionID, &hUserToken)) {
		writeToLogFile(L"GetSessionUserToken: Function success.");
		return TRUE;
	}
	writeToLogFile(L"GetSessionUserToken: Function false.");
	return FALSE;
}

BOOL startProcessAsCurrentUser(LPCWSTR appPath, LPCWSTR workDir, BOOL visible)
{
	HANDLE hUserToken = nullptr;
	STARTUPINFO startupInfo = { sizeof(STARTUPINFO) };
	PROCESS_INFORMATION procInfo;
	LPVOID pEnv = nullptr;
	//Define process creation flags(creationFlags) based on the required environmentand window display (visible).
	DWORD creationFlags = CREATE_UNICODE_ENVIRONMENT | (visible ? CREATE_NEW_CONSOLE : CREATE_NO_WINDOW);

	startupInfo.wShowWindow = visible ? SW_SHOWNORMAL : SW_HIDE;
	startupInfo.lpDesktop = (wchar_t*)(L"winsta0\\default");
	if (!getSessionUserToken(hUserToken))	//Get the user's token for the current session.
	{
		writeToLogFile(L"StartProcessAsCurrentUser: Failed to get user token.");
		return FALSE;
	}
	if (!CreateEnvironmentBlock(&pEnv, hUserToken, FALSE))	//Create the environment for the process
	{
		writeToLogFile(L"StartProcessAsCurrentUser: Failed to create environment block.");
		CloseHandle(hUserToken);
		return FALSE;
	}
	//Start the process under the session username
	if (!CreateProcessAsUser(hUserToken, appPath, NULL, NULL, NULL, FALSE, creationFlags, pEnv, workDir, &startupInfo, &procInfo))
	{
		DWORD error = GetLastError();
		wstring errorMessage = L"StartProcessAsCurrentUser: Failed to create process. Error Code: " + to_wstring(error);
		writeToLogFile(errorMessage);
		DestroyEnvironmentBlock(pEnv);	//Frees up resources that have been allocated to the environment.
		CloseHandle(hUserToken);
		return FALSE;
	}
	g_ProcessHandle = procInfo.hProcess;
	WaitForSingleObject(procInfo.hProcess, INFINITE);
	DestroyEnvironmentBlock(pEnv);	//Frees up resources that have been allocated to the environment.
	CloseHandle(procInfo.hThread);
	CloseHandle(procInfo.hProcess);
	CloseHandle(hUserToken);
	writeToLogFile(L"StartProcessAsCurrentUser: Function success");
	return TRUE;
}

wstring getPathFile(bool option)
{
	TCHAR szPath[MAX_PATH];
	wstring filePath;
	GetModuleFileName(NULL, szPath, MAX_PATH);
	wstring appPath(szPath);
	size_t slash = appPath.find_last_of(L"\\");
	if (slash != wstring::npos) {
		filePath = appPath.substr(0, slash + 1);
	}
	return option == true ? appPath : filePath;
}

#if _DEBUG
	void writeToLogFile(const wstring& logMessage)
	{
		wstring logFileName = getPathFile(false) + L"\\log.txt";
		HANDLE hFile = CreateFile(logFileName.c_str(), FILE_APPEND_DATA, FILE_SHARE_READ, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
		if (hFile != INVALID_HANDLE_VALUE) {
			wstring logMessageWithNewLine = logMessage + L"\r\n";
			DWORD bytesWritten = 0;
			WriteFile(hFile, logMessageWithNewLine.c_str(), static_cast<DWORD>(logMessageWithNewLine.length() * sizeof(wchar_t)), &bytesWritten, NULL);
			CloseHandle(hFile);
		}
	}
#endif
