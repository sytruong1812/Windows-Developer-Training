#include <Windows.h>
#include <iostream>
#include <string>
#include <tchar.h>
#include "HandleAutoRun.h"

bool handleStartup(const wstring& shortcutName, const wstring& path, const wstring& description)
{
	HRESULT hr = CoInitialize(NULL);
	if (FAILED(hr)) return false;

	IShellLink* pShellLink = NULL;
	hr = CoCreateInstance(CLSID_ShellLink, NULL, CLSCTX_ALL, IID_IShellLink, (LPVOID*)& pShellLink);
	if (SUCCEEDED(hr))
	{
		hr = pShellLink->SetPath(path.c_str());  // Path to the object we are referring to
		hr = pShellLink->SetDescription(description.c_str());
		hr = pShellLink->SetIconLocation(path.c_str(), 0);

		IPersistFile* pPersistFile;
		hr = pShellLink->QueryInterface(IID_IPersistFile, (LPVOID*)& pPersistFile);
		if (SUCCEEDED(hr))
		{
			WCHAR startupFolderPath[MAX_PATH];
			hr = SHGetFolderPathW(NULL, CSIDL_STARTUP, NULL, 0, startupFolderPath);
			if (SUCCEEDED(hr))
			{
				wcscat_s(startupFolderPath, shortcutName.c_str());
				hr = pPersistFile->Save(startupFolderPath, TRUE);
			}
			pPersistFile->Release();
		}
		pShellLink->Release();
	}
	CoUninitialize();
	return SUCCEEDED(hr);
}

bool handleRegistry(const wstring& valueName, const wstring& valueData, const wstring& subKey)
{
	HKEY hKey = nullptr;

	if (RegCreateKeyExW(HKEY_CURRENT_USER, subKey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_SET_VALUE, NULL, &hKey, NULL) == ERROR_SUCCESS) {
		if (RegSetValueExW(hKey, valueName.c_str(), 0, REG_SZ, (BYTE*)valueData.c_str(), (DWORD)(valueData.length() * sizeof(wchar_t))) == ERROR_SUCCESS) {
			RegCloseKey(hKey);
			return true;
		}
		RegCloseKey(hKey);
	}
	return false;
}

bool handleTaskSchd(HandleTask& task)
{
	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (FAILED(hr))
	{
		task.error = COINITIALIZEEX_FAILED;
		return false;
	}
	hr = CoInitializeSecurity(NULL, -1, NULL, NULL, RPC_C_AUTHN_LEVEL_PKT_PRIVACY, RPC_C_IMP_LEVEL_IMPERSONATE, NULL, 0, NULL);

	if (FAILED(hr))
	{
		task.error = COINITIALSECURITY_FAILED;
		CoUninitialize();
		return false;
	}
	ITaskService* pService = NULL;
	hr = CoCreateInstance(CLSID_TaskScheduler, NULL, CLSCTX_INPROC_SERVER, IID_ITaskService, (LPVOID*)& pService);
	if (FAILED(hr))
	{
		task.error = CREATE_INSTANCE_FAILED;
		CoUninitialize();
		return false;
	}
	hr = pService->Connect(_variant_t(), _variant_t(), _variant_t(), _variant_t());
	if (FAILED(hr))
	{
		task.error = CONNECT_FAILED;
		pService->Release();
		CoUninitialize();
		return false;
	}
	ITaskFolder* pRootFolder = NULL;
	hr = pService->GetFolder(_bstr_t(L"\\"), &pRootFolder);
	if (FAILED(hr))
	{
		task.error = GET_ROOT_FOLDER_FAILED;
		pService->Release();
		CoUninitialize();
		return false;
	}
	//  If the same task exists, remove it.
	pRootFolder->DeleteTask(_bstr_t(L"CryptoApp"), 0);
	ITaskDefinition* pTask = NULL;
	hr = pService->NewTask(0, &pTask);

	pService->Release();  // COM clean up.  Pointer is no longer used.
	if (FAILED(hr))
	{
		task.error = COCREATE_INSTANCE_FAILED;
		pRootFolder->Release();
		CoUninitialize();
		return false;
	}
	IRegistrationInfo* pRegInfo = NULL;
	hr = pTask->get_RegistrationInfo(&pRegInfo);
	if (FAILED(hr))
	{
		task.error = GET_IDENTIFICATION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pRegInfo->put_Author(_bstr_t("User"));
	pRegInfo->Release();
	if (FAILED(hr))
	{
		task.error = PUT_IDENTIFICATION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	/*==================================================================================*/
	//  Create the principal for the task - these credentials
	//  are overwritten with the credentials passed to RegisterTaskDefinition
	IPrincipal* pPrincipal = NULL;
	hr = pTask->get_Principal(&pPrincipal);
	if (FAILED(hr))
	{
		task.error = GET_PRINCIPAL_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pPrincipal->put_LogonType(TASK_LOGON_INTERACTIVE_TOKEN);
	//hr = pPrincipal->put_RunLevel(TASK_RUNLEVEL_LUA);

	pPrincipal->Release();
	if (FAILED(hr))
	{
		task.error = PUT_PRINCIPAL_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	/*=============================CREATE SETTING======================================*/
	ITaskSettings* pSettings = NULL;
	hr = pTask->get_Settings(&pSettings);
	if (FAILED(hr))
	{
		task.error = GET_SETTINGS_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pSettings->put_StartWhenAvailable(VARIANT_TRUE);
	pSettings->Release();
	if (FAILED(hr))
	{
		task.error = PUT_SETTINGS_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pSettings->put_DisallowStartIfOnBatteries(VARIANT_FALSE);
	if (FAILED(hr))
	{
		task.error = ERROR_PUT_DISALLOW_START;
		pSettings->Release();
		CoUninitialize();
		return false;
	}

	hr = pSettings->put_StopIfGoingOnBatteries(VARIANT_FALSE);
	if (FAILED(hr))
	{
		task.error = ERROR_PUT_STOP_ON_BATTERIES;
		pSettings->Release();
		CoUninitialize();
		return false;
	}
	IIdleSettings* pIdleSettings = NULL;
	hr = pSettings->get_IdleSettings(&pIdleSettings);
	if (FAILED(hr))
	{
		task.error = GET_IDLE_SETTINGS_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pIdleSettings->put_WaitTimeout(_bstr_t("PT5M"));
	pIdleSettings->Release();
	if (FAILED(hr))
	{
		task.error = PUT_IDLE_SETTINGS_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	/*================================SET ONE TIME======================================*/
	//  Get the trigger collection to insert the time trigger.
	ITriggerCollection* pTriggerCollection = NULL;
	hr = pTask->get_Triggers(&pTriggerCollection);
	if (FAILED(hr))
	{
		task.error = GET_TRIGGER_COLLECTION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	ITrigger* pTrigger = NULL;
	hr = pTriggerCollection->Create(TASK_TRIGGER_TIME, &pTrigger);
	pTriggerCollection->Release();
	if (FAILED(hr))
	{
		task.error = CREATE_TRIGGER_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	ITimeTrigger* pTimeTrigger = NULL;
	hr = pTrigger->QueryInterface(IID_ITimeTrigger, (void**)& pTimeTrigger);
	pTrigger->Release();
	if (FAILED(hr))
	{
		task.error = QUERYINTERFACE_TIMETRIGGER_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pTimeTrigger->put_Id(_bstr_t("MyTrigger"));
	if (FAILED(hr)) {
		task.error = PUT_TRIGGER_ID_FAILED;
	}
	wstring startTimes = GetCurrentSystemTime();
	hr = pTimeTrigger->put_StartBoundary(_bstr_t(startTimes.c_str()));
	hr = pTimeTrigger->put_RandomDelay(_bstr_t(L"PT1M"));

	pTimeTrigger->Release();
	if (FAILED(hr))
	{
		task.error = ADD_START_BOUNDARY_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	/*==================================REPEAT TASK=====================================*/
	IRepetitionPattern* pRepetitionPattern = NULL;
	hr = pTimeTrigger->get_Repetition(&pRepetitionPattern);
	if (FAILED(hr))
	{
		task.error = CANNOT_GET_REPETITION_PATTERN;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	// Set to repeat every 5 minutes (300 seconds)
	// Note that Interval and Duration are set using ISO 8601 time strings.
	// For example, "PT300S" is equivalent to repeating every 300 seconds (5 minutes).
	hr = pRepetitionPattern->put_Interval(_bstr_t(task.timeRepeat.c_str()));
	if (FAILED(hr))
	{
		task.error = CANNOT_SET_REPETITION_INTERVAL;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	// Set the number of repetitions: Set the repetition indefinitely by not 
	// calling the put_Duration() method.
	// hr = pRepetitionPattern->put_Duration(_bstr_t(L"P1D"));
	if (FAILED(hr))
	{
		task.error = CANNOT_SET_REPETITION_DURATION;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	pRepetitionPattern->Release();

	/*====================================ADD ACTION=====================================*/
	IActionCollection* pActionCollection = NULL;
	hr = pTask->get_Actions(&pActionCollection);
	if (FAILED(hr))
	{
		task.error = GET_TASK_COLLECTION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	IAction* pAction = NULL;
	hr = pActionCollection->Create(TASK_ACTION_EXEC, &pAction);
	pActionCollection->Release();
	if (FAILED(hr))
	{
		task.error = CREATE_ACTION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}

	IExecAction* pExecAction = NULL;
	hr = pAction->QueryInterface(
		IID_IExecAction, (void**)& pExecAction);
	pAction->Release();
	if (FAILED(hr))
	{
		task.error = QUERYINTERFACE_EXECACTION_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	hr = pExecAction->put_Path(_bstr_t(task.wstrExecutablePath.c_str()));
	pExecAction->Release();
	if (FAILED(hr))
	{
		task.error = PUT_ACTION_PATH_FAILED;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	/*===============================REGISTER TASK========================================*/
	IRegisteredTask* pRegisteredTask = NULL;
	hr = pRootFolder->RegisterTaskDefinition(
		_bstr_t(L"CryptoApp"),
		pTask,
		TASK_CREATE_OR_UPDATE,
		_variant_t(),
		_variant_t(),
		TASK_LOGON_INTERACTIVE_TOKEN,
		_variant_t(L""),
		&pRegisteredTask);
	if (FAILED(hr))
	{
		task.error = TASK_SAVE_ERROR;
		pRootFolder->Release();
		pTask->Release();
		CoUninitialize();
		return false;
	}
	//  Clean up.
	pRootFolder->Release();
	pTask->Release();
	pRegisteredTask->Release();
	CoUninitialize();
	return true;
}

bool SvcInstall(ServiceError& error)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	wstring Path = getPathFile();
	SERVICE_DELAYED_AUTO_START_INFO Info;
	Info.fDelayedAutostart = TRUE;
	schSCManager = OpenSCManager(	// Get a handle to the SCM database. 
		NULL,						// local computer
		NULL,						// ServicesActive database 
		SC_MANAGER_ALL_ACCESS);		// full access rights 
	if (schSCManager == NULL)
	{
		error = OPENSCMANAGER_FAILED;
		return false;
	}
	schService = CreateService(
		schSCManager,				// SCM database 
		SERVICE_NAME,				// name of service 
		SERVICE_NAME_DISPLAY,		// service name to display 
		SERVICE_ALL_ACCESS,			// desired access 
		SERVICE_WIN32_OWN_PROCESS | SERVICE_INTERACTIVE_PROCESS, // service type 
		SERVICE_AUTO_START,		    // start type 
		SERVICE_ERROR_NORMAL,       // error control type 
		Path.c_str(),			    // path to service's binary 
		NULL,                       // no load ordering group 
		NULL,                       // no tag identifier 
		NULL,                       // no dependencies 
		NULL,                       // LocalSystem account 
		NULL);                      // no password 

	if (ChangeServiceConfig2(schService, SERVICE_CONFIG_DELAYED_AUTO_START_INFO, &Info) == FALSE)
	{
		error = CREATESERVICE_FAILED;
		CloseServiceHandle(schService);
		CloseServiceHandle(schSCManager);
		return false;
	}
	if (schService == NULL)
	{
		error = CREATESERVICE_FAILED;
		CloseServiceHandle(schSCManager);
		return false;
	}

	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

bool SvcUninstall(ServiceError& error)
{
	SC_HANDLE schSCManager;
	SC_HANDLE schService;
	SERVICE_STATUS ssStatus = { 0 };
	schSCManager = OpenSCManager(	// Get a handle to the SCM database. 
		NULL,						// local computer
		NULL,						// ServicesActive database 
		SC_MANAGER_ALL_ACCESS);		// full access rights 
	if (schSCManager == NULL)
	{
		error = OPENSCMANAGER_FAILED;
		return false;
	}
	schService = OpenService(
		schSCManager,       // SCM database 
		SERVICE_NAME,       // name of service 
		DELETE);            // need delete access 
	if (schService == NULL)
	{
		error = OPENSERVICE_FAILED;
		CloseServiceHandle(schSCManager);
		return false;
	}
	if (!DeleteService(schService))	  // Delete the service.
	{
		error = DELETESERVICE_FAILED;
	}
	CloseServiceHandle(schService);
	CloseServiceHandle(schSCManager);
	return true;
}

wstring GetCurrentSystemTime()
{
	SYSTEMTIME systemTime;
	GetLocalTime(&systemTime);
	wchar_t buffer[20];  // "YYYY-MM-DDTHH:MM:SS" needs 19 characters + null terminator
	wsprintf(buffer, L"%04d-%02d-%02dT%02d:%02d:%02d",
		systemTime.wYear, systemTime.wMonth, systemTime.wDay,
		systemTime.wHour, systemTime.wMinute, systemTime.wSecond);

	return wstring(buffer);
}

wstring getPathFile()
{
	TCHAR szPath[MAX_PATH];
	GetModuleFileName(NULL, szPath, MAX_PATH);
	wstring modulePath(szPath);
	wstring filePath = (modulePath + L" --srv");
	return filePath;
}
