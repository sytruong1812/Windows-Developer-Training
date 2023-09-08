#include <iostream>
#include <string>
#include <windows.h>
#include "DynamicLib.h"
#include "StaticLib.h"

using namespace std;

bool inputOneNumber(double& num)
{
	while (true) {
		if (cin >> num) {
			if (cin.peek() == '\n') {
				cin.ignore(LLONG_MAX, '\n');
				return true;
			}
		}
		else {
			cout << "Invalid input! Enter again: ";
			cin.clear();
			cin.ignore(LLONG_MAX, '\n');
		}
	}
}

string toLowerCase(const string& str)
{
	string result;
	for (char c : str) {
		result += tolower(c);
	}
	return result;
}

double callFunctionFromDLL(double a, double b, const char* functionName)
{
	double result = 0.0;
	HMODULE hDll = LoadLibrary("DynamicLib.dll");
	if (hDll != NULL)
	{
		typedef double (*DLLFunction)(double, double);
		DLLFunction dllFunction = (DLLFunction)GetProcAddress(hDll, functionName);
		if (dllFunction != NULL)
		{
			result = dllFunction(a, b);
			FreeLibrary(hDll);
			return result;
		}
		else
		{
			cout << "Failed to load function from the DLL." << endl;
		}
		FreeLibrary(hDll);
	}
	else
	{
		cout << "Failed to load the DLL." << endl;
	}
	return result;
}

void mathProgram()
{
	bool again = true;
	do {
		double a = 0;
		double b = 0;
		double result = 0;
		char choose;
		string option;

		cout << "Enter number a: ";
		inputOneNumber(a);
		cout << "Enter number b: ";
		inputOneNumber(b);

		cout << "Add: + | Subtract: - | Multiply: * | Divide: /" << endl;
		cout << "Entern option: ";
		cin >> choose;
		switch (choose) {
		case '+':
			result = MathLibrary::Arithmetic::Add(a, b);
			if (DBL_MIN > result > DBL_MAX) {
				cout << "Result exceeds the maximum value representable by 'double'." << endl;
			}
			else {
				cout << "a + b = " << result << endl;
			}
			break;
		case '-':
			result = MathLibrary::Arithmetic::Subtract(a, b);
			if (DBL_MIN > result > DBL_MAX) {
				cout << "Result exceeds the maximum value representable by 'double'." << endl;
			}
			else {
				cout << "a - b = " << result << endl;
			}
			break;
		case '*':
			result = callFunctionFromDLL(a, b, "multiply");
			if (DBL_MIN > result > DBL_MAX) {
				cout << "Result exceeds the maximum value representable by 'double'." << endl;
			}
			else {
				cout << "a * b = " << result << endl;
			}
			break;
		case '/':
		{
			if (b == 0) {
				cout << "Cannot divide by zero! Please enter a non-zero value for b." << endl;
			}
			else {
				result = callFunctionFromDLL(a, b, "divide");
				if (DBL_MIN > result > DBL_MAX) {
					cout << "Result exceeds the maximum value representable by 'double'." << endl;
				}
				else {
					cout << "a / b = " << result << endl;
				}
			}
			break;
		}
		default:
			cout << "Invalid option, please try again." << endl;
			break;
		}
		bool againValid = true;
		while (againValid) {
			cout << "Continue: Yes | No" << endl;
			cout << "Enter option: ";
			cin >> option;
			if (toLowerCase(option) == "yes") {
				again = true;
				break;
			}
			else if (toLowerCase(option) == "no") {
				again = false;
				break;
			}
			else {
				cout << "Invalid option, please try again." << endl;
				againValid = false;
			}
		}
	} while (again);
}

int main()
{
	cout << "------Math Program-------" << endl;
	mathProgram();
	return 0;
}
