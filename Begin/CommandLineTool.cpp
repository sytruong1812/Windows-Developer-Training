#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <chrono>
using namespace std;

/*
*Function: searchPattern
*Discription: Search and compare each word in each line in file.text compare with pattern
*Idea: Find and compare 2 similar words need: equal size, equal value of each word
*Input:
*   path: The path to the file
*   pattern: string to look for in file
*Output: None
*/

void searchPattern1(const string path, const string pattern) {
	ifstream file(path, ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "Failed to open this file! Check the path again." << endl;
	}
	string line;
	int lineNumber = 0;
	//A loop to check each line in the file and save it to a line variable
	while (getline(file, line)) {
		lineNumber++;
		size_t patternLength = pattern.length();
		size_t lineLength = line.length();
		// Nested loop to compare each character in two string and pattern
		for (size_t i = 0; i <= lineLength - patternLength; i++) {
			// Variable bool to check the results of comparing each
			//character, conditions to print the result
			bool found = true;
			for (size_t j = 0; j < patternLength; j++) {
				if (line[i + j] != pattern[j]) {
					found = false;
					break;
				}
			}
			if (found == true) {
				cout << "Line " << lineNumber << ", character at " << i + 1 << endl;
			}
		}
	}
	if (lineNumber == 0) {
		cout << "No line found in file, file is empty!" << endl;
	}
	file.close();
}

void searchPattern2(const string& path, const string& pattern) {
	ifstream file(path, ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "Failed to open this file! Check the path again." << endl;
		return;
	}
	else {
		cout << "Opened file" << endl;
	}

	file.seekg(0, ios::end); // Đặt con trỏ đọc tới cuối file để lấy kích thước
	unsigned long long fileSize = file.tellg();		//8byte = 2^(8*8) 
	file.seekg(0, ios::beg); // Đặt con trỏ đọc lại về đầu file
	cout << "Size of file: " << fileSize << endl;

	char character;
	int lineNumber = 0;
	int characterCount = 0;
	int patternIndex = 0;
	size_t lengthPattern = pattern.length();
	/* A loop to check each character in the file and save it to a charater variable */
	while (file.get(character)) {
		characterCount++;
		/*Check the newline character to increment the line counter and reset the word counter*/
		if (character == '\n') {
			lineNumber++;
			characterCount = 0;
		}
		if (character == pattern[patternIndex]) {
			patternIndex++;
			if (patternIndex == lengthPattern) {
				cout << "Line " << lineNumber + 1 << ", character at " << (characterCount - lengthPattern + 1) << endl;
				patternIndex = 0;
			}
		}
		else {
			patternIndex = 0;
		}
	}
	if (lineNumber == 0 && fileSize == 0) {
		cout << "No line found in file, file is empty!" << endl;
	}
	file.close();
}

void searchPattern3(const string& path, const string& pattern) {

	ifstream file(path, ios::in | ios::binary);
	if (!file.is_open()) {
		cout << "Failed to open this file! Check the path again." << endl;
		return;
	}
	else {
		cout << "Opened file." << endl;
	}

	file.seekg(0, ios::end);
	unsigned long long fileSize = file.tellg();
	file.seekg(0, ios::beg);

	size_t bufferSize = 0;
	unsigned long long patternSize = pattern.length();

	if (fileSize == 0) {
		cout << "Empty file, check the file again!" << endl;
		return;
	}
	if (patternSize > fileSize) {
		cout << "Error!!! " << "Pattern size = " << patternSize << " > File size = " << fileSize << endl;
		return;
	}
	if (patternSize < fileSize) {
		bufferSize = 8200;
	}

	vector<char> buffer(bufferSize);

	int lineNumber = 0;
	int characterCount = 0;
	size_t patternIndex = 0;

	while (file) {
		file.read(buffer.data(), bufferSize); 
		unsigned long long bytesRead = file.gcount();
		for (size_t i = 0; i < bytesRead; i++) {
			characterCount++;
			if (buffer[i] == '\n') {
				lineNumber++;
				characterCount = 0;
			}
			if (buffer[i] == pattern[patternIndex]) {
				patternIndex++;
				if (patternIndex == patternSize) {
					cout << "Line " << lineNumber + 1 << ", character at " << (characterCount - patternSize + 1) << endl;
					patternIndex = 0;
				}
			}
			else {
				patternIndex = 0;
			}
		}
	}
	if (lineNumber == 0) {
		cout << "No line found in file! check the file again." << endl;
	}
	file.close();
}


int main(int argc, char* argv[])
{
	// Lưu thời điểm bắt đầu chương trình
	auto start = std::chrono::high_resolution_clock::now();

	searchPattern2("C:\\Users\\SY TRUONG\\Documents\\MicronSoftware\\Input\\1GB_file.txt", "Hello");

	//if (argc != 3) {
	//	cout << "Syntax Error!" << endl;
	//	cout << "<program> <path> <pattern>" << endl;
	//	return 0;
	//}
	//else {
	//	cout << "*-----Program Command Line-----*" << endl;
	//	searchPattern2(argv[1], argv[2]);
	//}

	 // Lưu thời điểm kết thúc chương trình
	auto end = std::chrono::high_resolution_clock::now();

	// Tính thời gian thực thi (tính bằng nanosecond)
	std::chrono::duration<double> duration = end - start;

	// Chuyển đổi đơn vị thời gian sang giây
	double durationInSeconds = duration.count();

	std::cout << "Thoi gian thuc thi: " << durationInSeconds << " giay" << std::endl;

	return 0;
}

