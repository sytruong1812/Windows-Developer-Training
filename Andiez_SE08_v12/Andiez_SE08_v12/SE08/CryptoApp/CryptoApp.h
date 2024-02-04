#pragma once
#include "resource.h"
#include "HandleCrypt.h"
#include "Initialize.h"

#define MAX_LOADSTRING	100
#define MIN_KEYLENGTH	5
#define MAX_KEYLENGTH	260

class CryptoApp
{
private:
	HWND hEdit;
	HWND hPathIn;
	HWND hPathOut;
	HWND hPathKey;
	HWND hCombox;
	HandleInfo info;
public:
	HINSTANCE hInst;
	HWND hWnd;
	HWND hDialogTask;
	HWND hDialogService;
	wstring timeRepeat;
	Algorithm getSelectedAlgorithm(Option myOption);

	CryptoApp();
	void createRunCloseButton();
	void createGroupMethod();
	void createGroupAlgorithm();
	void createInputPath();
	void createOutputPath();
	void createEnterKey();
	void createProgressBar();
	void createShowLogs();
	void createMenuOption();

	void createDialogBoxTask();
	void createStaticBoxTask();
	void createComboBoxTask();
	void createOkButtonTask();
	void createStaticTask();

	void createDialogBoxService();
	void createButtonService();
	void createStaticService();

	void handleButton(const int ID);
	void handleTextBox(const int ID);
	void handleBrowseFiles(const int ID);
	void handleComboBox(const int ID);
};