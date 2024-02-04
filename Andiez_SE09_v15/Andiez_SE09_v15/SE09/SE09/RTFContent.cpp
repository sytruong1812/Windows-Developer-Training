#include "RTFContent.h"

RTFContent::RTFContent()
{
	this->content = { 0 };
}

void RTFContent::setContent(const char ch)
{
	this->content += ch;
}

string RTFContent::getContent()
{
	return content;
}

void RTFContent::setAttribute(const string& key, const int value)
{
	attributes[key] = value;
}

int RTFContent::getAttribute(const string& key)
{
	auto it = attributes.find(key);
	if (it != attributes.end()) {
		return it->second;
	}
	return 0;
}
