#include "RTFReader.h"

RDS rds;
RIS ris;
static RTFGroup* currentGroup = nullptr;

RTFParse::RTFParse()
{
	this->nGroup = 0;
	this->state = STATE::sNorm;
	this->addObject = true;
	this->addAttribute = true;
	this->bytesRead = 0;
	this->hInputFile = nullptr;
	this->myHeader = new RTFHeader();
	this->myDocument = new RTFDocument();
}

RTFParse::~RTFParse()
{
	if (this->myHeader != nullptr) {
		delete this->myHeader;
		this->myHeader = nullptr;
	}
}

RTFHeader* RTFParse::getRTFHeader()
{
	return myHeader;
}

RTFDocument* RTFParse::getRTFDocument()
{
	return myDocument;
}

bool RTFParse::Parse(const wstring& inputPath, ERROR_PARSE& error)
{
	char ch = 0;
	int byte = 0;		//Hex = 1 BYTE
	int cNibble = 2;	//Count the number of hex digits remaining to complete a byte (2 hex digits make 1 byte).
	hInputFile = CreateFileW(inputPath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hInputFile == INVALID_HANDLE_VALUE) {
		error = ERROR_FILE_OPEN;
		return false;
	}
	while (ReadFile(hInputFile, &ch, 1, &bytesRead, NULL) && bytesRead > 0) {
		if (nGroup < 0) {
			error = ERROR_GET_NUM_GROUP;
			CloseHandle(hInputFile);
			return false;
		}
		if (ris == risBin) {
			//TODO: Handle parse character when in binary mode
		}
		switch (ch) {
		case '{':
			if (!pushRtfState()) {
				error = ERROR_PUSH_STATE;
				CloseHandle(hInputFile);
				return false;
			}
			break;
		case '}':
			if (!popRtfState()) {
				error = ERROR_POP_STATE;
				CloseHandle(hInputFile);
				return false;
			}
			break;
		case '\\':
			if (!parseKeyword(error)) {
				error = ERROR_PARSE_KEYWORD;
				CloseHandle(hInputFile);
				return false;
			}
			break;
		case 0x0d:
		case 0x0a:
			break;
		default:
			if (currentGroup != nullptr) {
				if (state == STATE::hFontTable) {
					static_cast<Fonttbl*>(currentGroup)->setTextGroup(ch);
				}
				else if (state == STATE::hStyleSheet) {
					static_cast<StyleSheet*>(currentGroup)->setTextGroup(ch);
				}
				else if (state == STATE::dParagraph) {
					wcout << ch;
					static_cast<Paragraph*>(currentGroup)->setTextGroup(ch);
				}
			}
			else {
				error = ERROR_NULL_PTR;
				CloseHandle(hInputFile);
				return false;
			}
			break;
		}
	}

	if (nGroup < 0 || nGroup > 0) {
		error = ERROR_GET_NUM_GROUP;
		CloseHandle(hInputFile);
		return false;
	}

	if (hInputFile != nullptr) {
		CloseHandle(hInputFile);
	}
	return true;
}

bool RTFParse::pushRtfState()
{
	SAVE pSaveNew;
	pSaveNew.rds = rds;
	pSaveNew.ris = ris;
	ris = RIS::risNorm;
	pSave.push(pSaveNew);
	nGroup++;
	return true;
}


bool RTFParse::popRtfState()
{
	if (pSave.empty()) {
		return false;
	}

	SAVE pSaveOld = pSave.top();
	pSave.pop();
	if (rds != pSaveOld.rds) {
		state = STATE::sNorm;
	}
	rds = pSaveOld.rds;
	ris = pSaveOld.ris;
	nGroup--;
	return true;
}

bool RTFParse::parseKeyword(ERROR_PARSE& error)
{
	char ch = 0;
	int param = 0;
	bool fParam = false;
	bool fNeg = false;
	char* pch = nullptr;
	char szKeyword[30];
	char* pKeywordMax = &szKeyword[30];
	char szParameter[20];
	char* pParamMax = &szParameter[20];
	szKeyword[0] = '\0';
	szParameter[0] = '\0';

	if (!ReadFile(hInputFile, &ch, 1, &bytesRead, NULL)) {
		error = ERROR_READ_DATA_FILE;
		return false;
	}
	if (!isalpha(ch)) {
		szKeyword[0] = ch;
		szKeyword[1] = '\0';
		PARAM myParam = { 0, fParam };
		return translateKeyword(szKeyword, myParam);
	}
	for (pch = szKeyword; pch < pKeywordMax && bytesRead > 0 && isalpha(ch); pch++) {
		*pch = ch;
		if (!ReadFile(hInputFile, &ch, 1, &bytesRead, NULL)) {
			error = ERROR_READ_DATA_FILE;
			return false;
		}
	}
	*pch = '\0';
	if (ch == '-') {
		fNeg = true;

		if (!ReadFile(hInputFile, &ch, 1, &bytesRead, NULL)) {
			error = ERROR_READ_DATA_FILE;
			return false;
		}
	}
	if (isdigit((int)ch)) {
		fParam = true;
		for (pch = szParameter; pch < pParamMax && bytesRead > 0 && isdigit(ch); pch++) {
			*pch = ch;
			if (!ReadFile(hInputFile, &ch, 1, &bytesRead, NULL)) {
				error = ERROR_READ_DATA_FILE;
				return false;
			}
		}
		*pch = '\0';
		param = atoi(szParameter);
		if (fNeg) {
			param = -param;
		}
	}
	if (ch != ' ') {
		SetFilePointer(hInputFile, -1, NULL, FILE_CURRENT);		// Restore the character to the file stream
	}
	PARAM myParam = { param, fParam };
	return translateKeyword(szKeyword, myParam);
}

//TODO: Separate Control Word types according to RTF_CTRLWORDS table and save
bool RTFParse::translateKeyword(const char* szKeyword, PARAM param)
{
	ControlType isym;
	bool found = false;
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
			handleRTFDestination(szKeyword, param);
		}
		break;
	case ControlType::Flag:
		if (strcmp(szKeyword, "pard") == 0) {
			state = STATE::dParagraph;
			if (addObject == true) {
				auto paragraph = new Paragraph("Paragraph", 0);
				myDocument->setParagraph(paragraph);
				myDocument->setAttribute("paragraph", param);
				addObject = false;
			}
			addAttribute = false;
		}
		break;
	case ControlType::Symbol:
		if (strcmp(szKeyword, "'") == 0) {
			ris = RIS::risHex;
		}
		else if (strcmp(szKeyword, "bin") == 0) {
			ris = RIS::risBin;
		}
		else {
			ris = RIS::risNorm;
		}
	case ControlType::Toggle:
	case ControlType::Value:
	case ControlType::Skip:
		break;
	default:
		break;
	}		
	handleSubGroup(szKeyword, param);
	//showProcess(state, szKeyword, param);
	return true;
}

void RTFParse::handleRTFDestination(const char* szKeyword, PARAM param)
{
	if (strcmp(szKeyword, "fonttbl") == 0) {		
		state = STATE::hFontTable;
		auto fonttbl = new Fonttbl(szKeyword, 0);
		myHeader->setFonttbl(fonttbl);
		myHeader->setAttribute(szKeyword, param);
		addAttribute = false;
	}
	else if (strcmp(szKeyword, "colortbl") == 0) {	
		state = STATE::hColorTable;
		auto colortbl = new Colortbl(szKeyword, 0);
		myHeader->setColortbl(colortbl);
		myHeader->setAttribute(szKeyword, param);
		addAttribute = false;
	}
	else if (strcmp(szKeyword, "filetbl") == 0) {	
		state = STATE::hFileTable;
		auto filetbl = new Filetbl(szKeyword, 0);
		myHeader->setFiletbl(filetbl);
		myHeader->setAttribute(szKeyword, param);
		addAttribute = false;
	}
	else if (strcmp(szKeyword, "stylesheet") == 0) {
		state = STATE::hStyleSheet;
		auto stylesheet = new StyleSheet(szKeyword, 0);
		myHeader->setStyleSheet(stylesheet);
		myHeader->setAttribute(szKeyword, param);
		addAttribute = false;
	}
	else if (strcmp(szKeyword, "listtable") == 0) {
		state = STATE::hListTable;
		auto listtable = new ListTable(szKeyword, 0);
		myHeader->setListTable(listtable);
		myHeader->setAttribute(szKeyword, param);
		addAttribute = false;
	}
	else if (strcmp(szKeyword, "info") == 0) {
		state = STATE::dInfo;
		auto info = new Info(szKeyword, 0);
		myDocument->setInfo(info);
		addAttribute = false;
	}
	else {
		state = STATE::sSkip;
	}
}

void RTFParse::handleSubGroup(const char* szKeyword, PARAM param)
{
	static int numOfSubFont = 0;
	static int numOfSubColor = 0;
	static int numOfSubFile = 0;
	static int numOfSubStyle = 0;
	static int numOfList = 0;
	static int numOfListLevel = 0;
	static int numOfPard = 0;
	static int temp = 0;
	
	switch (state) {
	case STATE::sNorm:
		if (strcmp(szKeyword, "*") != 0) {
			myHeader->setAttribute(szKeyword, param);
		}
		break;
	case STATE::sSkip:
		if (strcmp(szKeyword, "*") != 0) {
			myDocument->setAttribute(szKeyword, param);
		}
		break;
	case STATE::hFontTable:
		if (strcmp(szKeyword, "f") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getFonttbl();
			auto subGroupFont = new Fonttbl(keyword("f", param.num), numOfSubFont);
			currentGroup->setSubGroup(*subGroupFont);
			currentGroup = currentGroup->getSubGroups(numOfSubFont);
			numOfSubFont++;
		}
		break;
	case STATE::hColorTable:
		if (strcmp(szKeyword, "red") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getColortbl();
			auto subGroupColor = new Colortbl(keyword("cf", numOfSubColor), numOfSubColor);
			currentGroup->setSubGroup(*subGroupColor);
			currentGroup = currentGroup->getSubGroups(numOfSubColor);
			numOfSubColor++;
		}
		break;
	case STATE::hFileTable:
		if (strcmp(szKeyword, "fid") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getFiletbl();
			auto subGroupFile = new Filetbl(keyword("fid", numOfSubFile), numOfSubFile);
			currentGroup->setSubGroup(*subGroupFile);
			currentGroup = currentGroup->getSubGroups(numOfSubFile);
			numOfSubFile++;
		}
		break;
	case STATE::hStyleSheet:
		if (numOfSubStyle == 0 || strcmp(szKeyword, "s") == 0 || strcmp(szKeyword, "cs") == 0 || strcmp(szKeyword, "ts") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getStyleSheet();
			auto subGroupStyle = new StyleSheet(keyword(szKeyword, param.num), numOfSubStyle);
			currentGroup->setSubGroup(*subGroupStyle);
			currentGroup = currentGroup->getSubGroups(numOfSubStyle);
			numOfSubStyle++;
		}
		break;
	case STATE::hListTable:
		if (strcmp(szKeyword, "list") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getListTable();
			auto list = new ListTable(keyword(szKeyword, numOfList), numOfList);
			currentGroup->setSubGroup(*list);
			currentGroup = currentGroup->getSubGroups(numOfList);
			temp = numOfList;
			numOfList++;
			numOfListLevel = 0;		//Reset value = 0 when a new list is find
		}
		if (strcmp(szKeyword, "listlevel") == 0) {
			addAttribute = true;
			currentGroup = myHeader->getListTable();
			currentGroup = currentGroup->getSubGroups(temp);
			auto listlevel = new ListTable(keyword(szKeyword, numOfListLevel), numOfListLevel);
			currentGroup->setSubGroup(*listlevel);
			currentGroup = currentGroup->getSubGroups(numOfListLevel);
			numOfListLevel++;
		}
		break;
	case STATE::dParagraph:
		if (strcmp(szKeyword, "pard") == 0) {
			addAttribute = true;
			currentGroup = myDocument->getParagraph();
			auto pard = new Paragraph(keyword(szKeyword, numOfPard), numOfPard);
			currentGroup->setSubGroup(*pard);
			currentGroup = currentGroup->getSubGroups(numOfPard);
			numOfPard++;
		}
		break;
	default:
		break;
	}
	if (addAttribute && currentGroup != nullptr && strcmp(szKeyword, "*") != 0 && strcmp(szKeyword, "stylesheet") != 0) {
		currentGroup->setAttribute(szKeyword, param);
	}
}

void RTFParse::showProcess(STATE state, const char* szKeyword, PARAM param)
{
	if (state == STATE::sNorm) {
		cout << "STATE: Norm      " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::hFontTable) {
		cout << "STATE: Fonttbl   " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::hColorTable) {
		cout << "STATE: Colortbl  " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::hFileTable) {
		cout << "STATE: Filetbl   " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::hStyleSheet) {
		cout << "STATE: StyleSheet" << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::hListTable) {
		cout << "STATE: ListTable " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else if (state == STATE::dParagraph) {
		cout << "STATE: Paragraph " << " | Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
	else {
		cout << "Keyword: " << szKeyword << " | Param: " << param.num << endl;
	}
}
