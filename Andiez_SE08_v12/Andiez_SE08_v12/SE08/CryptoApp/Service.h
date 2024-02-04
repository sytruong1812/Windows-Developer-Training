#pragma once
#include <Shellapi.h>
#pragma comment(lib, "advapi32.lib")
#include <WtsApi32.h>
#pragma comment(lib, "Wtsapi32.lib")
#include <UserEnv.h>
#pragma comment(lib, "Userenv.lib")

#define SERVICE_NAME			_T("CryptoAppService")
#define SERVICE_NAME_DISPLAY	_T("CryptoApp Service")

using namespace std;

#if _DEBUG
void writeToLogFile(const wstring& logMessage);
#else
#define writeToLogFile(...)
#endif

wstring getPathFile(bool option);
VOID InitializeServiceStatus(DWORD controlAccept, DWORD currentState, DWORD exitCode, DWORD checkPoint);
VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode);
VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
BOOL getSessionUserToken(HANDLE& hUserToken);
BOOL startProcessAsCurrentUser(LPCWSTR appPath, LPCWSTR workDir, BOOL visible);