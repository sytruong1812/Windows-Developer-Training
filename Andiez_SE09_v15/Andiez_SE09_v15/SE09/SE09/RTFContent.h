#pragma once
#include "RTFType.h"

class RTFContent
{
private:
	string content;
	map<string, int> attributes;
public:
	RTFContent();
	void setContent(const char ch);
	string getContent();
	void setAttribute(const string& key, const int value);
	int getAttribute(const string& key);
};

