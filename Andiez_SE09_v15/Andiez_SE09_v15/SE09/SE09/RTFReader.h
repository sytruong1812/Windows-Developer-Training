#pragma once
#include "RTFGroup.h"

#define keyword(name, num) (string(name) + to_string(num))

class RTFReader
{
private:
	int nGroup;
	STATE state;
	SAVE* pSave = nullptr;
	RTFHeader* myHeader = nullptr;
	RTFDocument* myDocument = nullptr;
public:
	RTFReader();
	~RTFReader();
	bool Parse(const BYTE* buffer, size_t size);
	bool pushRtfState();
	bool popRtfState();
	bool parseKeyword(const BYTE* buffer, size_t& size, size_t& index);
	bool translateKeyword(char* szKeyword, int param, bool fParam);
	void handleRTFDestination(char* szKeyword, int param, bool fParam);
	void handleSubGroup(char* szKeyword, int param, bool fParam);
	void handleRTFInternal(char* szKeyword, int param, bool fParam);
	void showProcess(char* szKeyword, int param, bool fParam, STATE state);
};