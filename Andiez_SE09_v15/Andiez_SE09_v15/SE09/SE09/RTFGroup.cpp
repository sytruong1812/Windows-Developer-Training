#include "RTFGroup.h"

RTFGroup::RTFGroup(const string groupName)
{
	this->groupName = groupName;
	this->numOfGroup = 0;
}

RTFGroup::RTFGroup(const string groupName, const int numOfGroup)
{
	this->groupName = groupName;
	this->numOfGroup = numOfGroup;
}

void RTFGroup::setAttribute(const string& key, const int value)
{
	attributes[key] = value;
}

int RTFGroup::getAttribute(const string& key)
{
	auto it = attributes.find(key);
	if (it != attributes.end()) {
		return it->second;
	}
	return 0;
}

void RTFGroup::setSubGroup(const RTFGroup& group)
{
	subGroups.push_back(group);
}

string RTFGroup::getGroupName()
{
	return groupName;
}

void RTFGroup::setGroupName(const string& groupName)
{
	this->groupName = groupName;
}

int RTFGroup::getNumOfGroup()
{
	return numOfGroup;
}

void RTFGroup::setNumOfGroup(const int numOfGroup)
{
	this->numOfGroup = numOfGroup;
}

RTFGroup* RTFGroup::getSubGroups(const string& groupName, const int numOfGroup)
{
	for (RTFGroup& sub : subGroups) {
		if (sub.groupName == groupName && sub.numOfGroup == numOfGroup) {
			return &sub;
		}
	}
	return nullptr;
}


/*===========================HEADER============================*/
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

void RTFHeader::setAttribute(const string& key, const int value)
{
	myAttributes[key] = value;
}

int RTFHeader::getAttribute(const string& key)
{
	auto it = myAttributes.find(key);
	if (it != myAttributes.end()) {
		return it->second;
	}
	return 0;
}

/*========================DOCUMENT AREA======================*/

RTFDocument::RTFDocument()
{
	this->dParagraph = nullptr;
}

void RTFDocument::setParagraph(Paragraph* dParagraph)
{
	this->dParagraph = dParagraph;
}

Paragraph* RTFDocument::getParagraph()
{
	return dParagraph;
}