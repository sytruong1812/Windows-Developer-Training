#pragma once
#include "RTFGroup.h"

class Info : public RTFGroup
{
public:
	Info(const string name, const int num) : RTFGroup(name, num)
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

class Paragraph : public RTFGroup
{
private:
	//string font;
	//int size;
	//int lang;
	//int line;
public:
	Paragraph(const string name, const int num) : RTFGroup(name, num)
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
<document>	<info>? <docfmt>* <section>+
*/
class RTFDocument
{
private:
	Info* myInfo;
	Paragraph* myParagraph;
	list<pair<string, PARAM>> dAttributes;
public:
	RTFDocument();
	~RTFDocument();
	void setAttribute(const string& keyword, PARAM param);
	list<pair<string, PARAM>> getAttributes();
	void setInfo(Info* info);
	Info* getInfo();
	void setParagraph(Paragraph* paragraph);
	Paragraph* getParagraph();
};
