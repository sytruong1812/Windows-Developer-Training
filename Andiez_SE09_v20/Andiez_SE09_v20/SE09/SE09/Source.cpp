#include "RTFReader.h"
#include "RTFWriter.h"

using namespace std;

int wmain(int argc, const wchar_t* argv[])
{
	wstring inputFilePath = argv[1];	//L"C:\\Users\\home\\Downloads\\inputFile.rtf";
	wstring outputFilePath = argv[2];	//L"C:\\Users\\home\\Downloads\\outputFile.rtf";
	ERROR_PARSE errorParse;

	RTFParse myRTFParse;
	RTFMerge myRTFMerge;

	if (!myRTFParse.Parse(inputFilePath, errorParse)) {
		cout << MessagesParse[errorParse].c_str() << endl;
	}
	else {
		cout << "\nParsed RTF file successfully" << endl;
		myRTFMerge.Merge(outputFilePath, myRTFParse.getRTFHeader(), myRTFParse.getRTFDocument());
	}

	return 0;
}