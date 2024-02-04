#include "RTFDocument.h"

RTFDocument::RTFDocument()
{
	myInfo = nullptr;
	myParagraph = nullptr;
}

RTFDocument::~RTFDocument()
{
	if (myInfo != nullptr) {
		delete myInfo;
	}
	else if (myParagraph != nullptr) {
		delete myParagraph;
	}
}

void RTFDocument::setAttribute(const string& keyword, PARAM param)
{
	dAttributes.push_back(make_pair(keyword, param));
}

list<pair<string, PARAM>> RTFDocument::getAttributes()
{
	return dAttributes;
}

void RTFDocument::setInfo(Info* info)
{
	this->myInfo = info;
}
Info* RTFDocument::getInfo()
{
	return myInfo;
}

void RTFDocument::setParagraph(Paragraph* paragraph)
{
	this->myParagraph = paragraph;
}
Paragraph* RTFDocument::getParagraph()
{
	return myParagraph;
}
