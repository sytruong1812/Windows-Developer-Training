#pragma once
#include "RTFGroup.h"
#include "RTFHeader.h"
#include "RTFDocument.h"

enum ERROR_PARSE
{
	ERROR_FILE_OPEN,
	ERROR_READ_DATA_FILE,
	ERROR_NULL_PTR,
	ERROR_GET_NUM_GROUP,
	ERROR_PUSH_STATE,
	ERROR_POP_STATE,
	ERROR_PARSE_KEYWORD,
	ERROR_HEX_CHAR,
};

const string MessagesParse[] = {
	"Error: Can't open file."
	"Error: Can't read data form file."
	"Error: Can't access in the address was delete.",
	"Error: Can't get number of group."
	"Error: Push status is faulty.",
	"Error: Pop status is faulty.",
	"Error: Can't parse keyword.",
	"Error: Hex character found is invalid",
};

class RTFParse
{
private:
	int nGroup;
	STATE state;
	stack<SAVE> pSave;
	bool addObject;
	bool addAttribute;
	DWORD bytesRead;
	HANDLE hInputFile;
	RTFHeader* myHeader = nullptr;
	RTFDocument* myDocument = nullptr;
public:
	RTFParse();
	~RTFParse();
	RTFHeader* getRTFHeader();
	RTFDocument* getRTFDocument();
	bool Parse(const wstring& inputPath, ERROR_PARSE& error);
private:
	bool popRtfState();
	bool pushRtfState();
	bool parseKeyword(ERROR_PARSE& error);
	bool translateKeyword(const char* szKeyword, PARAM param);
	void handleRTFDestination(const char* szKeyword, PARAM param);
	void handleSubGroup(const char* szKeyword, PARAM param);
	void showProcess(STATE state, const char* szKeyword, PARAM param);
};