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
	HandleInfo info;
public:
	HWND hWnd;
	HINSTANCE hInst;

	CryptoApp();
	void createRunCloseButton();
	void createGroupMethod();
	void createGroupAlgorithm();
	void createInputPath();
	void createOutputPath();
	void createEnterKey();
	void createProgressBar();
	void createShowLogs();

	void handleButton(const int ID);
	void handleTextBox(const int ID);
	void handleBrowseFiles(const int ID);

	Algorithm getSelectedAlgorithm(Option myOption);
};