#pragma once
#include <string>
#include <comdef.h>
#include <wincred.h>
#include <taskschd.h>

#pragma comment(lib, "taskschd.lib")
#pragma comment(lib, "comsupp.lib")
#pragma comment(lib, "credui.lib")

using namespace std;

enum ERROR_HANDLE_TASKSCHD
{
	NONE,
	COINITIALIZEEX_FAILED,
	COINITIALSECURITY_FAILED,
	CREATE_INSTANCE_FAILED,
	CONNECT_FAILED,
	GET_ROOT_FOLDER_FAILED,
	COCREATE_INSTANCE_FAILED,
	GET_IDENTIFICATION_FAILED,
	PUT_IDENTIFICATION_FAILED,
	GET_PRINCIPAL_FAILED,
	PUT_PRINCIPAL_FAILED,
	GET_SETTINGS_FAILED,
	PUT_SETTINGS_FAILED,
	GET_IDLE_SETTINGS_FAILED,
	PUT_IDLE_SETTINGS_FAILED,
	GET_TRIGGER_COLLECTION_FAILED,
	CREATE_TRIGGER_FAILED,
	QUERYINTERFACE_TIMETRIGGER_FAILED,
	PUT_TRIGGER_ID_FAILED,
	PUT_TRIGGER_BOUNDARY_FAILED,
	ADD_START_BOUNDARY_FAILED,
	GET_TASK_COLLECTION_FAILED,
	CREATE_ACTION_FAILED,
	QUERYINTERFACE_EXECACTION_FAILED,
	PUT_ACTION_PATH_FAILED,
	TASK_SAVE_ERROR,
	CANNOT_GET_REPETITION_PATTERN,
	CANNOT_SET_REPETITION_INTERVAL,
	CANNOT_SET_REPETITION_DURATION,
	ERROR_PUT_DISALLOW_START,
	ERROR_PUT_STOP_ON_BATTERIES
};

const wstring MessagesErrorTaskSchd[] = {
	L"None",
	L"CoInitializeEx failed",
	L"CoInitializeSecurity failed",
	L"Failed to create an instance of ITaskService",
	L"ITaskService::Connect failed",
	L"Cannot get Root folder pointer",
	L"Failed to CoCreate an instance of the TaskService class",
	L"Cannot get identification pointer",
	L"Cannot put identification info",
	L"Cannot get principal pointer",
	L"Cannot put principal info",
	L"Cannot get settings pointer",
	L"Cannot put setting information",
	L"Cannot get idle setting information",
	L"Cannot put idle setting information",
	L"Cannot get trigger collection",
	L"Cannot create trigger",
	L"QueryInterface call failed for ITimeTrigger",
	L"Cannot put trigger ID",
	L"Cannot put end boundary on trigger",
	L"Cannot add start boundary to trigger",
	L"Cannot get Task collection pointer",
	L"Cannot create the action",
	L"QueryInterface call failed for IExecAction",
	L"Cannot put action path",
	L"Error saving the Task"
	L"Cannot get the repetition pattern.",
	L"Cannot set the repetition interval.",
	L"Cannot set the repetition duration.",
	L"Failed to get DisallowStartIfOnBatteries.",
	L"Failed to put StopIfGoingOnBatteries."
};

struct HandleTask
{
	wstring taskName = L"Crypto";
	wstring timeRepeat = L"PT5M";
	wstring wstrExecutablePath = { 0 };
	ERROR_HANDLE_TASKSCHD error = NONE;
};

wstring GetCurrentSystemTime();
bool createTaskSchd(HandleTask& task);