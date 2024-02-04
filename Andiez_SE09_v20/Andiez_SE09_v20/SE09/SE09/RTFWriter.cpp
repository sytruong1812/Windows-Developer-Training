#include "RTFWriter.h"

void RTFMerge::Merge(const wstring& outputPath, RTFHeader* myHeader, RTFDocument* myDocument)
{
	hOutputFile = CreateFileW(outputPath.c_str(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hOutputFile == INVALID_HANDLE_VALUE) {
		return;
	}

	writeRTFHeader(myHeader);
	writeRTFDocument(myDocument);

	if (hOutputFile != nullptr) {
		CloseHandle(hOutputFile);
	}
}

void RTFMerge::writeRTFHeader(RTFHeader* myHeader)
{
	ctrlword = myHeader->getAttributes();
	for (const auto& in : ctrlword) {
		key = in.first;
		value = in.second;

		if (strcmp(key.c_str(), "fonttbl") == 0) {
			writeFonttbl(myHeader->getFonttbl());
		}
		else if (strcmp(key.c_str(), "colortbl") == 0) {
			writeColortbl(myHeader->getColortbl());
		}
		else if (strcmp(key.c_str(), "filetbl") == 0) {
			//writeFiletbl(myHeader->getFiletbl());
		}
		else if (strcmp(key.c_str(), "stylesheet") == 0) {
			writeStylesheet(myHeader->getStyleSheet());
		}
		else if (strcmp(key.c_str(), "listtable") == 0) {
			//writeListTable(myHeader->getListTable());
		}
		else {
			if (strcmp(key.c_str(), "rtf") == 0) {
				attribute = "{\\" + keyword(key, value.num);;
			}
			else if (value.fParam) {
				attribute = "\\" + keyword(key, value.num);
			}
			else {
				attribute = "\\" + key;
			}
		}
		WriteFile(hOutputFile, attribute.c_str(), static_cast<DWORD>(attribute.size()), &bytesWritten, NULL);
		attribute.clear();
	}
}

void RTFMerge::writeRTFDocument(RTFDocument* myDocument)
{
	ctrlword = myDocument->getAttributes();
	for (const auto& in : ctrlword) {
		key = in.first;
		value = in.second;

		if (strcmp(key.c_str(), "info") == 0) {

		}
		else if (strcmp(key.c_str(), "paragraph") == 0) {
			writeParagraph(myDocument->getParagraph());
		}
		else {
			if (value.fParam) {
				attribute = "\\" + keyword(key, value.num);
			}
			else {
				attribute = "\\" + key;
			}
		}
		temp += attribute;
		attribute.clear();
	}
	temp += "}";
	WriteFile(hOutputFile, temp.c_str(), static_cast<DWORD>(temp.size()), &bytesWritten, NULL);
	temp.clear();
}


void RTFMerge::writeFonttbl(Fonttbl* myFonttbl)
{
	numSub = myFonttbl->getNumOfSubGroup();

	for (int i = 0; i < numSub; i++) {
		auto mySubFont = (Fonttbl*)myFonttbl->getSubGroups(i);
		for (const auto& pair : mySubFont->getAttribute()) {
			if (pair.second.fParam) {
				attribute += "\\" + pair.first + to_string(pair.second.num);
			}
			else {
				attribute += "\\" + pair.first;
			}
		}
		if (!des) {
			table += "\n{\\fonttbl{" + attribute + " " + mySubFont->getTextGroup() + "}";
			des = true;
		}
		else {
			table += "{" + attribute + " " + mySubFont->getTextGroup() + "}";
		}
		if (i == numSub - 1) {
			table += "}";
		}
		WriteFile(hOutputFile, table.c_str(), static_cast<DWORD>(table.size()), &bytesWritten, NULL);
		attribute.clear();
		table.clear();
	}
	des = false;
}

void RTFMerge::writeColortbl(Colortbl* myColortbl)
{
	numSub = myColortbl->getNumOfSubGroup();
	
	for (int i = 0; i < numSub; i++) {
		auto mySubColor = (Colortbl*)myColortbl->getSubGroups(i);
		for (const auto& pair : mySubColor->getAttribute()) {
			if (pair.second.fParam) {
				attribute += "\\" + pair.first + to_string(pair.second.num);
			}
			else {
				attribute += "\\" + pair.first;
			}
		}
		if (!des) {
			table += "\n{\\colortbl ;" + attribute + ";";
			des = true;
		}
		else {
			table += attribute +  ";}";
		}
		WriteFile(hOutputFile, table.c_str(), static_cast<DWORD>(table.size()), &bytesWritten, NULL);
		attribute.clear();
		table.clear();
	}
	des = false;
}

void RTFMerge::writeFiletbl(Filetbl* myFiletbl)
{

}

void RTFMerge::writeStylesheet(StyleSheet* myStylesheet)
{
	numSub = myStylesheet->getNumOfSubGroup();

	for (int i = 0; i < numSub; i++) {
		auto mySubStyle = (StyleSheet*)myStylesheet->getSubGroups(i);
		for (const auto& pair : mySubStyle->getAttribute()) {
			if (pair.second.fParam) {
				if (strcmp(pair.first.c_str(), "cs") == 0) {
					attribute += "\\*\\" + pair.first + to_string(pair.second.num);
				}
				else {
					attribute += "\\" + pair.first + to_string(pair.second.num);
				}
			}
			else {
				attribute += "\\" + pair.first;
			}
		}
		if (!des) {
			table += "\n{\\stylesheet{" + attribute + " " + mySubStyle->getTextGroup() + "}";
			des = true;
		}
		else {
			table += "{" + attribute + " " + mySubStyle->getTextGroup() + "}";
		}
		temp += table;
		attribute.clear();
		table.clear();
	}
	temp += "}";
	WriteFile(hOutputFile, temp.c_str(), static_cast<DWORD>(temp.size()), &bytesWritten, NULL);
	temp.clear();
	des = false;
}

void RTFMerge::writeListTable(ListTable* myListTable)
{
	bool addMore = true;
	numSub = myListTable->getNumOfSubGroup();
	for (int i = 0; i < numSub; i++) {
		auto mySubStyle = (StyleSheet*)myListTable->getSubGroups(i);
		int numStyle = mySubStyle->getNumOfSubGroup();
		for (const auto& pair : mySubStyle->getAttribute()) {		// \list\listtemplateid67698721
			if (pair.second.fParam) {
				attribute += "\\" + pair.first + to_string(pair.second.num);
			}
			else {
				attribute += "\\" + pair.first;
			}
		}
		for (int i = 0; i < numStyle; i++) {
			auto mySubLevel = (StyleSheet*)mySubStyle->getSubGroups(i);
			for (const auto& pair : mySubLevel->getAttribute()) {
				if (pair.second.fParam) {
					attribute += "\\" + pair.first + to_string(pair.second.num);
				}
				else {
					if (strcmp(pair.first.c_str(), "listlevel") == 0) {
						attribute += "{\\" + pair.first;
					}
					else if (strcmp(pair.first.c_str(), "leveltext") == 0) {
						attribute += "{\\" + pair.first;
					}
					else if (strcmp(pair.first.c_str(), "levelnumbers") == 0) {
						attribute += ";}{\\" + pair.first + ";}";
					}
					else if (strcmp(pair.first.c_str(), "listname") == 0) {
						attribute += "}{\\" + pair.first + ";}";
						addMore = false;
					}
					else {
						attribute += "\\" + pair.first;
					}
				}
			}
			if (addMore) {
				attribute += "}";
			}
		}
		addMore = true;
		if (!des) {
			table += "{\\*\\listtable{" + attribute + " " + mySubStyle->getTextGroup() + "}";
			des = true;
		}
		else {
			table += "{" + attribute + " " + mySubStyle->getTextGroup() + "}";
		}

		WriteFile(hOutputFile, table.c_str(), static_cast<DWORD>(table.size()), &bytesWritten, NULL);
		attribute.clear();
		table.clear();
	}
	des = false;
}

void RTFMerge::writeParagraph(Paragraph* myParagraph)
{
	numSub = myParagraph->getNumOfSubGroup();

	for (int i = 0; i < numSub; i++) {
		auto mySubPard = (Fonttbl*)myParagraph->getSubGroups(i);
		for (const auto& pair : mySubPard->getAttribute()) {
			if (pair.second.fParam) {
				attribute += "\\" + pair.first + to_string(pair.second.num);
			}
			else {
				if (strcmp(pair.first.c_str(), "par") == 0) {
					attribute += " " + mySubPard->getTextGroup();
				}
				attribute += "\\" + pair.first;
			}
		}
		table += "\n" + attribute;
		WriteFile(hOutputFile, table.c_str(), static_cast<DWORD>(table.size()), &bytesWritten, NULL);
		attribute.clear();
		table.clear();
	}
	des = false;
}
