#include "RTFGroup.h"

RTFGroup::RTFGroup(const string name, const int num)
{
	this->name = name;
	this->num = num;
}

string RTFGroup::getGroupName()
{
	return name;
}

void RTFGroup::setGroupName(const string& name)
{
	this->name = name;
}

int RTFGroup::getNumOfGroup()
{
	return num;
}

void RTFGroup::setNumOfGroup(const int num)
{
	this->num = num;
}

RTFGroup* RTFGroup::getSubGroups(const int num)
{
	for (RTFGroup& sub : subGroups) {
		if (sub.num == num) {
			return &sub;
		}
	}
	return nullptr;
}

void RTFGroup::setSubGroup(const RTFGroup& group)
{
	subGroups.push_back(group);
}

int RTFGroup::getNumOfSubGroup()
{
	return (int)subGroups.size();
}

list<pair<string, PARAM>> RTFGroup::getAttribute()
{
	return attributes;
}

void RTFGroup::setAttribute(const string& keyword, PARAM param)
{
	attributes.push_back(make_pair(keyword, param));
}
