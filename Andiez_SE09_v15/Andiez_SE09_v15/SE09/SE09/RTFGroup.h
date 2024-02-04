#pragma once
#include "RTFType.h"

class RTFGroup
{
protected:
	string groupName;
	int numOfGroup;
	vector<RTFGroup> subGroups;
	map<string, int> attributes;
public:
	RTFGroup(const string groupName);
	RTFGroup(const string groupName, const int numOfGroup);

	string getGroupName();
	void setGroupName(const string& groupName);

	int getNumOfGroup();
	void setNumOfGroup(const int numOfGroup);

	void setSubGroup(const RTFGroup& group);
	RTFGroup* getSubGroups(const string& groupName, const int numOfGroup);

	void setAttribute(const string& key, const int value);
	int getAttribute(const string& key);
};

/*Is - a*/
class Fonttbl : public RTFGroup
{
private:
	string text;
public:
	Fonttbl(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	Fonttbl(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
	void setTextGroup(const char ch)
	{
		this->text += ch;
	}
	string getTextGroup()
	{
		return text;
	}
};

/*Is - a*/
class Colortbl : public RTFGroup {
public:
	Colortbl(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	Colortbl(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
};

/*Is - a*/
class Filetbl : public RTFGroup {
private:
	string text;
public:
	Filetbl(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	Filetbl(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
	void setTextGroup(const char ch)
	{
		this->text += ch;
	}
	string getTextGroup()
	{
		return text;
	}
};

/*Is - a*/
class StyleSheet : public RTFGroup
{
private:
	string text;
public:
	StyleSheet(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	StyleSheet(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
	void setTextGroup(const char ch)
	{
		this->text += ch;
	}
	string getTextGroup()
	{
		return text;
	}
};

/*Is - a*/
class ListTable : public RTFGroup
{
private:
	string text;
public:
	ListTable(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	ListTable(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
	void setTextGroup(const char ch)
	{
		this->text += ch;
	}
	string getTextGroup()
	{
		return text;
	}
};

/*
<header>\rtf <charset> \deff? <fonttbl> <filetbl>? <colortbl>? <stylesheet>? <listtables>? <revtbl>?
*/
class RTFHeader
{
private:
	/*Has - a*/
	Fonttbl* myFonttbl;
	Colortbl* myColortbl;
	Filetbl* myFiletbl;
	StyleSheet* myStylesheet;
	ListTable* myListTable;
	map<string, int> myAttributes;
public:
	RTFHeader();
	~RTFHeader();
	void setAttribute(const string& key, const int value);
	int getAttribute(const string& key);

	void setFonttbl(Fonttbl* fonttbl);
	Fonttbl* getFonttbl();
	void setColortbl(Colortbl* colortbl);
	Colortbl* getColortbl();
	void setFiletbl(Filetbl* filetbl);
	Filetbl* getFiletbl();
	void setStyleSheet(StyleSheet* stylesheet);
	StyleSheet* getStyleSheet();
	void setListTable(ListTable* listtable);
	ListTable* getListTable();
};

/*Is- a*/
class Paragraph : public RTFGroup
{
private:
	string text;
public:
	Paragraph(const string groupName) : RTFGroup(groupName)
	{
		this->groupName = groupName;
	}
	Paragraph(const string groupName, const int numOfGroup) : RTFGroup(groupName, numOfGroup)
	{
		this->groupName = groupName;
		this->numOfGroup = numOfGroup;
	}
	void setTextGroup(const char ch)
	{
		this->text += ch;
	}
	string getTextGroup()
	{
		return text;
	}
};

/*
<document>	<info>? <docfmt>* <section>+
*/
class RTFDocument
{
private:
	/*Has - a*/
	Paragraph* dParagraph;
	map<string, int> hAttributes;
public:
	RTFDocument();
	void setParagraph(Paragraph* dParagraph);
	Paragraph* getParagraph();

};
