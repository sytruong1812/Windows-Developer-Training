#pragma once
#include <string>

using namespace std;

class RTFColor
{
private:
	int red;
	int green;
	int blue;
public:
	static const RTFColor Black;
	static const RTFColor White;

	RTFColor(int red, int green, int blue);
	int getRed() const;
	void setRed() const;

	int getGreen() const;
	void setGreen() const;

	int getBlue() const;
	void setBlue() const;

	string showColor() const;
};