#pragma once
#include "RTFGroup.h"

class Fonttbl : public RTFGroup
{
public:
	Fonttbl(const string name, const int num) : RTFGroup(name, num)
	{
		this->name = name;
		this->num = num;
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

class Colortbl : public RTFGroup
{
public:
	Colortbl(const string name, const int num) : RTFGroup(name, num)
	{
		this->name = name;
		this->num = num;
	}
};

class Filetbl : public RTFGroup
{
public:
	Filetbl(const string name, const int num) : RTFGroup(name, num)
	{
		this->name = name;
		this->num = num;
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

class StyleSheet : public RTFGroup
{
public:
	StyleSheet(const string name, const int num) : RTFGroup(name, num)
	{
		this->name = name;
		this->num = num;
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

class ListTable : public RTFGroup
{
public:
	ListTable(const string name, const int num) : RTFGroup(name, num)
	{
		this->name = name;
		this->num = num;
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
	Fonttbl* myFonttbl;
	Colortbl* myColortbl;
	Filetbl* myFiletbl;
	StyleSheet* myStylesheet;
	ListTable* myListTable;
	list<pair<string, PARAM>> hAttributes;
public:
	RTFHeader();
	~RTFHeader();
	void setAttribute(const string& keyword, PARAM param);
	list<pair<string, PARAM>> getAttributes();
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