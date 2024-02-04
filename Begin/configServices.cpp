#include <windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include <strsafe.h>

#pragma comment(lib, "advapi32.lib")

using namespace std;

SERVICE_STATUS        g_ServiceStatus = { 0 };
SERVICE_STATUS_HANDLE g_StatusHandle = NULL;
HANDLE                g_ServiceStopEvent = INVALID_HANDLE_VALUE;

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv);
VOID WINAPI ServiceCtrlHandler(DWORD);
DWORD WINAPI ServiceWorkerThread(LPVOID lpParam);

#define SERVICE_NAME  _T("Notepad Service")


//int main()
//{
//	SC_HANDLE schSCManager = OpenSCManager(NULL, NULL, SC_MANAGER_CREATE_SERVICE);
//	if (schSCManager)
//	{
//		/*=======================================Create==================================*/
//		SC_HANDLE schService = CreateService(
//			schSCManager,
//			"NotepadService",         // Name of service
//			"Notepad Service",       // Name to display
//			SERVICE_ALL_ACCESS,				// Desired access
//			SERVICE_WIN32_OWN_PROCESS,  // Service type
//			SERVICE_AUTO_START,         // Start type
//			SERVICE_ERROR_NORMAL,       // Error control type
//			"C:\\Windows\\notepad.exe", // Path to the service executable
//			NULL,
//			NULL,
//			NULL,
//			NULL,
//			NULL);
//
//		if (schService)
//		{
//			cout << "Service created successfully." << endl;
//			CloseServiceHandle(schService);
//		}
//		else
//		{
//			cout << "Failed to create service. Error: " << GetLastError() << endl;
//		}
//
//		/*=======================================Delete==================================*/
//		SC_HANDLE schService = OpenService(schSCManager, "NotepadService", DELETE);
//		if (schService)
//		{
//			if (DeleteService(schService))
//			{
//				std::cout << "Service deleted successfully." << std::endl;
//			}
//			else
//			{
//				std::cout << "Failed to delete service. Error: " << GetLastError() << std::endl;
//			}
//			CloseServiceHandle(schService);
//		}
//		else
//		{
//			std::cout << "Failed to open service. Error: " << GetLastError() << std::endl;
//		}
//		CloseServiceHandle(schSCManager);
//	}
//	else
//	{
//		cout << "Failed to open service control manager. Error: " << GetLastError() << endl;
//	}
//	return 0;
//}

int _tmain(int argc, TCHAR* argv[])
{
	SERVICE_TABLE_ENTRY ServiceTable[] =
	{
		{(LPSTR)SERVICE_NAME, (LPSERVICE_MAIN_FUNCTION)ServiceMain},
		{NULL, NULL}
	};

	if (StartServiceCtrlDispatcher(ServiceTable) == FALSE)
	{
		cout << "False" << endl;
		return GetLastError();
	}

	return 0;
}

VOID WINAPI ServiceMain(DWORD argc, LPTSTR* argv)
{
	DWORD Status = E_FAIL;

	// Register our service control handler with the SCM
	g_StatusHandle = RegisterServiceCtrlHandler(SERVICE_NAME, ServiceCtrlHandler);

	if (g_StatusHandle == NULL)
	{
		return;
	}

	// Tell the service controller we are starting
	ZeroMemory(&g_ServiceStatus, sizeof(g_ServiceStatus));
	g_ServiceStatus.dwServiceType = SERVICE_WIN32_OWN_PROCESS;
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_START_PENDING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwServiceSpecificExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}

	/*
	 * Perform tasks necessary to start the service here
	 */

	 // Create a service stop event to wait on later
	g_ServiceStopEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
	if (g_ServiceStopEvent == NULL)
	{
		// Error creating event
		// Tell service controller we are stopped and exit
		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
		g_ServiceStatus.dwWin32ExitCode = GetLastError();
		g_ServiceStatus.dwCheckPoint = 1;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(_T(
				"My Sample Service: ServiceMain: SetServiceStatus returned error"));
		}
		return;
	}

	// Tell the service controller we are started
	g_ServiceStatus.dwControlsAccepted = SERVICE_ACCEPT_STOP;
	g_ServiceStatus.dwCurrentState = SERVICE_RUNNING;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 0;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}

	// Start a thread that will perform the main task of the service
	HANDLE hThread = CreateThread(NULL, 0, ServiceWorkerThread, NULL, 0, NULL);

	// Wait until our worker thread exits signaling that the service needs to stop
	WaitForSingleObject(hThread, INFINITE);


	/*
	 * Perform any cleanup tasks
	 */

	CloseHandle(g_ServiceStopEvent);

	// Tell the service controller we are stopped
	g_ServiceStatus.dwControlsAccepted = 0;
	g_ServiceStatus.dwCurrentState = SERVICE_STOPPED;
	g_ServiceStatus.dwWin32ExitCode = 0;
	g_ServiceStatus.dwCheckPoint = 3;

	if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
	{
		OutputDebugString(_T(
			"My Sample Service: ServiceMain: SetServiceStatus returned error"));
	}	
}

VOID WINAPI ServiceCtrlHandler(DWORD CtrlCode)
{
	switch (CtrlCode)
	{
	case SERVICE_CONTROL_STOP:

		if (g_ServiceStatus.dwCurrentState != SERVICE_RUNNING)
			break;

		/*
		 * Perform tasks necessary to stop the service here
		 */

		g_ServiceStatus.dwControlsAccepted = 0;
		g_ServiceStatus.dwCurrentState = SERVICE_STOP_PENDING;
		g_ServiceStatus.dwWin32ExitCode = 0;
		g_ServiceStatus.dwCheckPoint = 4;

		if (SetServiceStatus(g_StatusHandle, &g_ServiceStatus) == FALSE)
		{
			OutputDebugString(_T(
				"My Sample Service: ServiceCtrlHandler: SetServiceStatus returned error"));
		}

		// This will signal the worker thread to start shutting down
		SetEvent(g_ServiceStopEvent);

		break;

	default:
		break;
	}
}

DWORD WINAPI ServiceWorkerThread(LPVOID lpParam)
{
	//  Periodically check if the service has been requested to stop
	while (WaitForSingleObject(g_ServiceStopEvent, 0) != WAIT_OBJECT_0)
	{
		/*
		 * Perform main service function here
		 */

		 //  Simulate some work by sleeping
		Sleep(3000);
	}

	return ERROR_SUCCESS;
}
