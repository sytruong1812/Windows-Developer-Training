#include "RTFFile.h"
#include "RTFReader.h"

using namespace std;

int main(){
	RTFReader myRTF;
	wstring path = L"C:\\Users\\home\\Downloads\\file.rtf";
	FileProcessError error;
	BYTE* buffer = nullptr;
	size_t size = 0;

	if (readFileRTF(path, buffer, size, error)) {
		if (!myRTF.Parse(buffer, size)) {
			cout << "\nError parsing RTF" << endl;
		}
		else {
			cout << "\nParsed RTF file successfully" << endl;
		}
	}
	else {
		cout << MessagesError[error].c_str() << endl;
	}
	return 0;
}
