#pragma once
#include "RTFGroup.h"
#include "RTFHeader.h"
#include "RTFDocument.h"

class RTFMerge
{
private:
	bool des;
	int numSub;
	string key;
	PARAM value;
	string temp;
	string table;
	string attribute;
	HANDLE hOutputFile;
	DWORD bytesWritten;
	list<pair<string, PARAM>> ctrlword;
public:
	void Merge(const wstring& outputPath, RTFHeader* myHeader, RTFDocument* myDocument);
	void writeRTFHeader(RTFHeader* myHeader);
	void writeRTFDocument(RTFDocument* myDocument);
	void writeFonttbl(Fonttbl* myFonttbl);
	void writeColortbl(Colortbl* myColortbl);
	void writeFiletbl(Filetbl* myFiletbl);
	void writeStylesheet(StyleSheet* myStylesheet);
	void writeListTable(ListTable* myListTable);
	void writeParagraph(Paragraph* myParagraph);
};