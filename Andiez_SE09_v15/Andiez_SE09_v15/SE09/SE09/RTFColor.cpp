#include <stdexcept>
#include "RTFColor.h"

RTFColor::RTFColor(int red, int green, int blue)
{
	this->red = red;
	this->green = green;
	this->blue = blue;
	if (red < 0 || red > 255 || green < 0 || green > 255 || blue < 0 || blue > 255) {
		throw invalid_argument("Invalid color values.");
	}
}

int RTFColor::getRed() const
{
	return red;
}

int RTFColor::getGreen() const
{
	return green;
}

int RTFColor::getBlue() const
{
	return blue;
}

string RTFColor::showColor() const
{
	return "Color {" + to_string(red) + "," + to_string(green) + "," + to_string(blue) + "}";
}

const RTFColor RTFColor::Black(0, 0, 0);
const RTFColor RTFColor::White(255, 255, 255);

