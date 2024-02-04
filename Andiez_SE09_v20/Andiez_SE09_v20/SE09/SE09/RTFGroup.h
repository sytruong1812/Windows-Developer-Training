#pragma once
#include "RTFType.h"

class RTFGroup
{
protected:
	string name;
	int num;
	string text;
	vector<RTFGroup> subGroups;
	list<pair<string, PARAM>> attributes;
public:
	RTFGroup(const string name, const int num);
	string getGroupName();
	void setGroupName(const string& name);
	int getNumOfGroup();
	void setNumOfGroup(const int num);
	RTFGroup* getSubGroups(const int num);
	void setSubGroup(const RTFGroup& group);
	int getNumOfSubGroup();
	list<pair<string, PARAM>> getAttribute();
	void setAttribute(const string& keyword, PARAM param);
};