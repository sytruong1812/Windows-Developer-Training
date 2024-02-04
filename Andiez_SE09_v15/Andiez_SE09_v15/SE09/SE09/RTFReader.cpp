#include "RTFReader.h"

RDS rds;
RIS ris;
static RTFGroup* currentGroup = nullptr;

Fonttbl* fonttbl = nullptr;
Colortbl* colortbl = nullptr;
Filetbl* filetbl = nullptr;
StyleSheet* stylesheet = nullptr;
ListTable* listtable = nullptr;

Paragraph* paragraph = nullptr;

RTFReader::RTFReader()
{
	this->nGroup = 0;
	this->state = STATE::sNorm;
	this->myHeader = new RTFHeader();
	this->myDocument = new RTFDocument();
}

RTFReader::~RTFReader()
{
	if (this->myHeader != nullptr) {
		delete this->myHeader;
		this->myHeader = nullptr;
	}
}

bool RTFReader::Parse(const BYTE* buffer, size_t size)
{
	size_t index = 0;
	char ch;
	while (index < size) {
		ch = buffer[index++];
		if (nGroup < 0) {
			return false;
		}
		switch (ch) {
		case '{':
			if (!pushRtfState()) {
				return false;
			}
			break;
		case '}':
			if (!popRtfState()) {
				return false;
			}
			break;
		case '\\':
			if (!parseKeyword(buffer, size, index)) {
				return false;
			}
			break;
		case 0x0d:		//CR: Move the pointer to the begin line 
		case 0x0a:		//LB: Move the pointer to the new line
			break;
		default:
			//TODO: Save content
			if (rds == rdsNorm) {
				cout << ch;
			}
			if (state == STATE::hFontTable && ch != ';') {
				((Fonttbl*)currentGroup)->setTextGroup(ch);
			}
			if (state == STATE::hStyleSheet && ch != ';') {
				((StyleSheet*)currentGroup)->setTextGroup(ch);
			}
			if (state == STATE::dParagraph && ch != ';') {
				((Paragraph*)currentGroup)->setTextGroup(ch);
			}
			break;
		}
	}
	if (nGroup < 0) {
		return false;
	}
	if (nGroup > 0) {
		return false;
	}
	return true;
}

bool RTFReader::pushRtfState()
{
	SAVE* pSaveNew = new SAVE;
	if (!pSaveNew) {
		return false;
	}
	pSaveNew->pNext = pSave;

	pSaveNew->rds = rds;
	pSaveNew->ris = ris;

	pSave = pSaveNew;
	nGroup++;
	return true;
}

bool RTFReader::popRtfState()
{
	SAVE* pSaveOld;
	if (!pSave) {
		return false;
	}
	if (rds != pSave->rds) {	//TODO: Handle when exit a group (eg: Save fonttbl)
		state = STATE::sNorm;
	}
	rds = pSave->rds;
	ris = pSave->ris;

	pSaveOld = pSave;
	pSave = pSave->pNext;

	nGroup--;
	delete(pSaveOld);
	return true;
}

bool RTFReader::parseKeyword(const BYTE* buffer, size_t& size, size_t& index)
{
	char ch = buffer[index++];
	char szKeyword[30];
	int param = 0;
	bool fParam = false;
	char szParameter[20];
	bool fNeg = false;
	char* pch = nullptr;
	char* pParamMax = &szParameter[20];
	szKeyword[0] = '\0';
	szParameter[0] = '\0';

	if (index >= size) {
		return false;
	}
	if (!isalpha(ch)) {
		szKeyword[0] = ch;
		szKeyword[1] = '\0';
		return translateKeyword(szKeyword, 0, fParam);
	}
	// Get keyword: Assign character to szKeyword aray and move the pointer to the next position
	for (pch = szKeyword; pch < szKeyword + 30 && isalpha(ch); ch = buffer[index++]) {
		*pch++ = ch;
		if (index >= size) {
			return false;
		}
	}
	if (pch >= szKeyword + 30) {
		return false;
	}
	*pch = '\0';
	if (ch == '-') {
		fNeg = true;
		ch = buffer[index++];
		if (index >= size) {
			return false;
		}
	}
	// Get param: Assign param to szParameter array and move the pointer to the next position
	if (isdigit(ch)) {
		fParam = true;
		for (pch = szParameter; pch < pParamMax && isdigit(ch); ch = buffer[index++]) {
			*pch++ = ch;
			if (index >= size) {
				return false;
			}
		}
		if (pch >= pParamMax) {
			return false;
		}
		*pch = '\0';
		param = atoi(szParameter);
		if (fNeg) {
			param = -param;
		}
	}
	if (ch != ' ') {
		index--;
	}
	return translateKeyword(szKeyword, param, fParam);
}

//TODO: Separate Control Word types according to RTF_CTRLWORDS table and save
bool RTFReader::translateKeyword(char* szKeyword, int param, bool fParam)
{
	bool found = false;
	ControlType isym;
	for (const auto& pair : TABLE_CTRLWORDS) {
		if (strcmp(szKeyword, pair.first.c_str()) == 0) {
			found = true;
			isym = pair.second;
			break;
		}
	}
	if (!found) {
		isym = ControlType::Skip;
	}

	switch (isym)
	{
	case ControlType::Destination:
		if (strcmp(szKeyword, "rtf") != 0) {
			rds = rdsSkip;
			handleRTFDestination(szKeyword, param, fParam);
		}
		break;
	case ControlType::NF:
		rds = rdsSkip;
		break;
	case ControlType::Flag:
		if (strcmp(szKeyword, "pard") == 0 || strcmp(szKeyword, "info") == 0) {			// \Content
			state = STATE::dParagraph;
			paragraph = new Paragraph("Paragraph");
			myDocument->setParagraph(paragraph);
		}
	case ControlType::Symbol:
	case ControlType::Toggle:
	case ControlType::Value:
	case ControlType::Skip:
		break;
	default:
		break;
	}
	handleSubGroup(szKeyword, param, fParam);
	showProcess(szKeyword, param, fParam, state);
	return true;
}

//TODO: Processing here.... Handle RTF Destination
void RTFReader::handleRTFDestination(char* szKeyword, int param, bool fParam)
{

	if (strcmp(szKeyword, "fonttbl") == 0) {		
		state = STATE::hFontTable;
		fonttbl = new Fonttbl("Fonttbl");
		myHeader->setFonttbl(fonttbl);
	}
	else if (strcmp(szKeyword, "colortbl") == 0) {	
		state = STATE::hColorTable;
		colortbl = new Colortbl("Colortbl");
		myHeader->setColortbl(colortbl);
	}
	else if (strcmp(szKeyword, "filetbl") == 0) {	
		state = STATE::hFileTable;
		filetbl = new Filetbl("Filetbl");
		myHeader->setFiletbl(filetbl);
	}
	else if (strcmp(szKeyword, "stylesheet") == 0) {
		state = STATE::hStyleSheet;
		stylesheet = new StyleSheet("Stylesheet");
		myHeader->setStyleSheet(stylesheet);
	}
	else if (strcmp(szKeyword, "listtable") == 0) {
		state = STATE::hListTable;
		listtable = new ListTable("ListTable");
		myHeader->setListTable(listtable);
	}
}

//TODO: Handle subGroup in the RTF Destination
void RTFReader::handleSubGroup(char* szKeyword, int param, bool fParam)
{
	static int numOfSubColor = 0;
	static int numOfSubFile = 0;
	static int numOfSubStyle = 0;
	static int numOfParagraph = 0;
	static int temp = 0;
	
	switch (state) {
	case STATE::sNorm:
		myHeader->setAttribute(szKeyword, param);
		break;
	case STATE::hFontTable:
		if (strcmp(szKeyword, "f") == 0) {
			currentGroup = myHeader->getFonttbl();
			auto subGroupFont = new Fonttbl("f", param);
			fonttbl->setSubGroup(*subGroupFont);
			currentGroup = fonttbl->getSubGroups("f", param);			// /f0, /f1...
		}
		break;
	case STATE::hColorTable:
		if (strcmp(szKeyword, "red") == 0) {
			currentGroup = myHeader->getColortbl();
			auto subGroupColor = new Colortbl("cf", numOfSubColor);
			colortbl->setSubGroup(*subGroupColor);
			currentGroup = colortbl->getSubGroups("cf", numOfSubColor);	// /cf1, /cf2, ...
			numOfSubColor++;
		}
		break;
	case STATE::hFileTable:
		if (strcmp(szKeyword, "fid") == 0) {
			currentGroup = myHeader->getFiletbl();
			auto subGroupFile = new Filetbl("fid", numOfSubFile);
			filetbl->setSubGroup(*subGroupFile);
			currentGroup = filetbl->getSubGroups("fid", numOfSubFile);	// /fi1, /fi2, ...
			numOfSubFile++;
		}
		break;
	case STATE::hStyleSheet:
		if (strcmp(szKeyword, "s") == 0 || strcmp(szKeyword, "cs") == 0 || strcmp(szKeyword, "ts") == 0 || numOfSubStyle == 0) {
			currentGroup = myHeader->getStyleSheet();
			auto subGroupStyle = new StyleSheet(szKeyword, param);
			stylesheet->setSubGroup(*subGroupStyle);
			currentGroup = stylesheet->getSubGroups(szKeyword, param);	// /s1, /cs2, ...
			numOfSubStyle++;
		}
		break;
	case STATE::hListTable:
		if (strcmp(szKeyword, "list") == 0 || strcmp(szKeyword, "listpicture") == 0 || strcmp(szKeyword, "shppict") == 0) {
			currentGroup = myHeader->getListTable();
			auto list = new ListTable(szKeyword, param);
			listtable->setSubGroup(*list);
			currentGroup = listtable->getSubGroups(szKeyword, param);
		}
		break;
	default:
		break;
	}

	if (strcmp(szKeyword, "*") != 0 && currentGroup != nullptr) {
		currentGroup->setAttribute(szKeyword, param);
	}
}

void RTFReader::handleRTFInternal(char* szKeyword, int param, bool fParam){}

void RTFReader::showProcess(char* szKeyword, int param, bool fParam, STATE state)
{
	if (state == STATE::sNorm) {
		cout << "STATE: Norm" << " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::hFontTable) {
		cout << "STATE: Fonttbl" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::hColorTable) {
		cout << "STATE: Colortbl" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::hFileTable) {
		cout << "STATE: Filetbl" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::hStyleSheet) {
		cout << "STATE: StyleSheet" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::hListTable) {
		cout << "STATE: ListTable" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else if (state == STATE::dParagraph) {
		cout << "STATE: Paragraph" " | Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
	else {
		cout << "Keyword: " << szKeyword << " | Param: " << param << " | fParam: " << (fParam ? "True" : "False") << endl;
	}
}
