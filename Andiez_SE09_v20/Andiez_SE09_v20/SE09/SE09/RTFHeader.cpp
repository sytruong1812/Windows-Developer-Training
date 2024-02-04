#include "RTFHeader.h"

RTFHeader::RTFHeader()
{
	myFonttbl = nullptr;
	myColortbl = nullptr;
	myFiletbl = nullptr;
	myStylesheet = nullptr;
	myListTable = nullptr;
}

RTFHeader::~RTFHeader()
{
	if (myFonttbl != nullptr) {
		delete myFonttbl;
	}
	else if (myColortbl != nullptr) {
		delete myColortbl;
	}
	else if (myFiletbl != nullptr) {
		delete myFiletbl;
	}
	else if (myStylesheet != nullptr) {
		delete myStylesheet;
	}
	else if (myListTable != nullptr) {
		delete myListTable;
	}
}

void RTFHeader::setFonttbl(Fonttbl* fonttbl)
{
	myFonttbl = fonttbl;
}
Fonttbl* RTFHeader::getFonttbl()
{
	return myFonttbl;
}

void RTFHeader::setColortbl(Colortbl* colortbl)
{
	myColortbl = colortbl;
}
Colortbl* RTFHeader::getColortbl()
{
	return myColortbl;
}

void RTFHeader::setFiletbl(Filetbl* filetbl)
{
	myFiletbl = filetbl;
}
Filetbl* RTFHeader::getFiletbl()
{
	return myFiletbl;
}

void RTFHeader::setStyleSheet(StyleSheet* stylesheet)
{
	myStylesheet = stylesheet;
}
StyleSheet* RTFHeader::getStyleSheet()
{
	return myStylesheet;
}

void RTFHeader::setListTable(ListTable* listtable)
{
	myListTable = listtable;
}
ListTable* RTFHeader::getListTable()
{
	return myListTable;
}

void RTFHeader::setAttribute(const string& keyword, PARAM param)
{
	hAttributes.push_back(make_pair(keyword, param));
}

list<pair<string, PARAM>> RTFHeader::getAttributes()
{
	return hAttributes;
}