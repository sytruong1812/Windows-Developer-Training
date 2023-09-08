#include <iostream>

#define LENGTH_CMD 8191

using namespace std;

int stringLength(const char* ptr) {
	int countWord = 0;
	while (*ptr != '\0') {
		countWord++;
		ptr++;
	}
	return countWord;
}

bool isStringEmpty(const char* str) {
	int length = stringLength(str);

	if (length == 0) {
		return true;
	}
	for (int i = 0; i < length; i++) {
		if (str[i] != ' ' && str[i] != '\t') {
			return false;
		}
	}
	// for (int i = length - 1; i >= 0; i--) {
	// 	if (str[i] != ' ' && str[i] != '\t') {
	// 		return false;
	// 	}
	// }
	return true;
}

void searchString(const char* array1, const int &len1, const char* array2, const int &len2) {
	int count = 0;
	for (int i = 0; i <= len1 - len2; i++) {
		bool found = true;
		for (int j = 0; j < len2; j++) {
			if (array1[i + j] != array2[j]) {
				found = false;
				break;
			}
		}
		if (found == true) {
			cout << "Characters found at: " << i + 1 << endl;
			count++;
		}
	}
	if (count == 0) {
		cout << "Position not found!" << endl;
	}
}

int main() {
	cout << "Program to input two strings" << endl;
	char* array1 = new char[LENGTH_CMD];
	char* array2 = new char[LENGTH_CMD];
	int len1, len2;
	bool again = false;
	while(again == false){
		cout << "Enter string one: ";
		cin.getline(array1, LENGTH_CMD);
		len1 = stringLength(array1);

		cout << "Enter string two: ";
		cin.getline(array2, LENGTH_CMD);
		len2 = stringLength(array2);

		if (isStringEmpty(array1) && isStringEmpty(array2)) {
			cout << "Both strings are empty! Cannot perform search, enter again." << endl;
			again = false;
		}
		else if (isStringEmpty(array1)) {
			cout << "String 1 is empty! Cannot perform search, enter again." << endl;
			again = false;
		}
		else if (isStringEmpty(array2)) {
			cout << "String 2 is empty! Cannot perform search, enter again." << endl;
			again = false;
		}
		else if (len1 < len2) {
			cout << "Error: String 1 < String 2! Do not search on a string longer than another." << endl;
			again = false;
		}
		else {
			searchString(array1, len1, array2, len2);
			break;
		}
	}
	delete[] array1;
	delete[] array2;
	return 0;
}