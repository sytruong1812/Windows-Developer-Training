#include <iostream>
#include <Windows.h>
#include <wininet.h>
#include <vector>
#include <string>
#include <sstream>
#include <Shlwapi.h>

#define KB 1024
#define MAXTHREADS 5

using namespace std;

size_t totalDataDownloaded = 0;
HANDLE globalConsoleHandle = nullptr;
HANDLE semaphore = nullptr;
HANDLE mutex = nullptr;

enum ChooseConsole
{
	SHOW_THREAD,
	SHOW_OPTION,
	SHOW_DUBLINK,
	SHOW_WAIT
};

enum StatusDownload
{
	START,
	DOWNLOADING,
	COMPLETED,
	CANCELLED,
	ERROR_DOWNLOAD
};

enum ErrorDownload
{
	NONE,
	INTERNET_OPEN_ERROR,
	FILE_ATTRIBUTES_ERROR,
	CREATE_FILE_ERROR,
	HTTP_ERROR,
	HTTP_404_CODE,
	NEW_BUFFER_FALSE,
	DELETE_FILE_ERROR,
	WRITE_FILE_ERROR,
	FILE_EMPTY,
	INTERNET_READ_FILE_FAILED
};

const string MessagesError[] = {
	"No thread information found.",
	"Unable to open internet connection!",
	"The file or directory does not exist or cannot be accessed!",
	"Unable to create file!",
	"Can't get information from server.",
	"File not found in directory.",
	"Memory allocation failed.",
	"Failed to delete file.",
	"WriteFile process failed!",
	"Get file size returns empty size!",
	"Network error, file reading failed!",
};

const string MessagesStatus[] = {
	"Start",
	"Downloading",
	"Completed",
	"Cancelled",
	"Error download",
};

struct ThreadInfo
{
	int progress = 0;
	size_t numThread = 0;
	DWORD threadId = 0;
	DWORD fileSize = 0;
	wstring url = { 0 };
	wstring filePath = { 0 };
	wstring fileName = { 0 };
	bool overwrite = false;
	bool cancelEvent = false;
	HANDLE threadHandle = nullptr;
	HANDLE threadMutex = nullptr;
	HINTERNET hInternet = nullptr;
	ErrorDownload error = NONE;
	StatusDownload status = START;
};

void clearFromLineToEnd(const size_t startLine, const size_t subjectLine, const size_t num)
{
	cout << "\033[" << startLine + subjectLine + num << ";0H\033[J";
}

string toLowerCase(const string& str)
{
	string result;
	for (char c : str) {
		result += tolower(c);
	}
	return result;
}

bool inputString(string& str)
{
	getline(cin >> ws, str);
	if (!str.empty() && str.find(' ') == string::npos) {
		return true;
	}
	else {
		return false;
	}
}

bool inputNumber(size_t& num)
{
	bool result = true;
	if (cin >> num) {
		if (cin.peek() == '\n' && num > 0) {
			return true;
		}
		else {
			result = false;
		}
	}
	else {
		result = false;
	}
	cin.clear();
	cin.ignore(LLONG_MAX, '\n');
	return result;
}

bool createNestedDir(const wstring& path)
{
	if (path.size() > MAX_PATH)
	{
		return false;
	}

	if (PathFileExistsW(path.c_str())) {
		return true;
	}
	else {
		size_t lastSeparatorPos = path.find_last_of(L"\\");
		if (lastSeparatorPos != wstring::npos && lastSeparatorPos > 1) {
			wstring parentPath = path.substr(0, lastSeparatorPos);
			if (createNestedDir(parentPath)) {
				return CreateDirectoryW(path.c_str(), NULL);
			}
		}
		return false;
	}
}

wstring getFileNameFromUrl(const wstring& url, const size_t num)
{
	wstring newFileName = { 0 };
	size_t isLastSlash = url.find_last_of(L"/");
	if (isLastSlash != wstring::npos && isLastSlash < url.length() - 1) {
		return url.substr(isLastSlash + 1);
	}
	newFileName = L"file(" + to_wstring(num) + L").bin";
	return newFileName;
}

void writeToConsole(ChooseConsole choose, const size_t num, const ThreadInfo * thread)
{
	DWORD length = 0;
	COORD coord;
	COORD coordPointer;
	size_t row = 0;
	wstring option;
	wstringstream out;
	HANDLE hConsole = globalConsoleHandle;

	switch (choose) {
	case ChooseConsole::SHOW_THREAD:
		row = thread->numThread;
		coord = { static_cast<SHORT>(0), static_cast<SHORT>(row + 1) };
		option = thread->overwrite ? L"True" : L"False";
		out << "Index : " << row + 1;
		if (thread->error == INTERNET_OPEN_ERROR && thread->status == ERROR_DOWNLOAD) {
			out << " | Error: Network connection error! Check your server. | " << thread->url;
		}
		else if (thread->error == HTTP_404_CODE && thread->status == ERROR_DOWNLOAD) {
			out << " | Error 404: File not found. | " << thread->url;
		}
		else {
			out << " | ID: " << thread->threadId
				<< " | Download: " << thread->progress << " %"
				<< " | " << thread->fileSize << " Byte"
				<< " | " << thread->url
				<< " | Overwrite: " << option;
		}
		WriteConsoleOutputCharacterW(hConsole, out.str().c_str(), (DWORD)out.str().length(), coord, &length);
		break;
	case ChooseConsole::SHOW_OPTION:
		coord = { static_cast<SHORT>(0), static_cast<SHORT>(num + 2) };
		coordPointer = { static_cast<SHORT>(0) , static_cast<SHORT>(num + 3) };
		out << "*----My option [Show|Cancel|Exit]----*";
		WriteConsoleOutputCharacterW(hConsole, out.str().c_str(), (DWORD)out.str().length(), coord, &length);
		SetConsoleCursorPosition(hConsole, coordPointer);
		break;
	case ChooseConsole::SHOW_DUBLINK:
		coord = { static_cast<SHORT>(0) , static_cast<SHORT>(num + 1) };
		SetConsoleCursorPosition(hConsole, coord);
		wcout << "Index : " << num + 1 << " | You entered a duplicate link, will only download 1 file." << endl;
		break;
	case ChooseConsole::SHOW_WAIT:
		coord = { static_cast<SHORT>(0) , static_cast<SHORT>(num + 1) };
		SetConsoleCursorPosition(hConsole, coord);
		wcout << "Index : " << num + 1 << " | Thread is waiting to start." << endl;
		break;
	default:
		cout << "First argument error!" << endl;
		return;
	}
}

void showStateThread(const ThreadInfo & info)
{
	wcout << "Index : " << info.numThread + 1
		<< " | ID: " << info.threadId
		<< " | Status: " << MessagesStatus[info.status].c_str()
		<< " | Progress: " << info.progress << "%"
		<< " | File Size: " << info.fileSize << " Byte" << "\n\t ";
	if (info.status != StatusDownload::ERROR_DOWNLOAD) {
		wcout << " | URL: " << info.url
			<< " | File Name: " << info.fileName
			<< " | File Path: " << info.filePath << endl;
	}
	else {
		wcout << " | Error: " << MessagesError[info.error].c_str() << endl;
	}

}

void showThreadList(const vector<ThreadInfo*> & threads, const size_t sameNumberFile)
{
	if (threads.empty()) {
		cout << "No running threads found." << endl;
		return;
	}
	clearFromLineToEnd(threads.size(), 4, sameNumberFile);
	cout << "*-----------List of threads----------*" << endl;
	for (ThreadInfo* info : threads) {
		showStateThread(*info);
	}
}

void cancelDownload(vector<ThreadInfo*> & threads, const size_t sameNumberFile)
{
	size_t num = 0;
	clearFromLineToEnd(threads.size(), 4, sameNumberFile);
	while (true) {
		cout << "Enter number of thread you want to cancel: ";
		if (inputNumber(num) == true) {
			if (num > (threads.size() + sameNumberFile)) {
				cout << "Error: Please enter valid positive integer and correct thread number" << endl;
			}
			else {
				break;
			}
		}
		else {
			cout << "Error: Input must be a number that matches the number of thread!" << endl;
		}
	}
	for (ThreadInfo* thread : threads) {
		if ((size_t)thread->numThread + 1 == num) {
			switch (thread->status) {
			case StatusDownload::START:
				cout << "Wait for the file downloader to run!" << endl;
				break;
			case StatusDownload::COMPLETED:
				cout << "This thread has already completed!" << endl;
				break;
			case StatusDownload::CANCELLED:
				cout << "This thread has cancelled!" << endl;
				break;
			case StatusDownload::ERROR_DOWNLOAD:
				cout << "You can't cancel a thread that hasn't started yet!" << endl;
				break;
			default:
				if (thread->threadHandle) {
					thread->cancelEvent = true;
					break;
				}
			}
			break;
		}
	}
}

void updateStatusAndError(ThreadInfo * info, ErrorDownload error, StatusDownload status)
{
	WaitForSingleObject(info->threadMutex, INFINITE);		// Wait for a permission from the mutex
	info->error = error;
	info->status = status;
	ReleaseMutex(info->threadMutex);						// Release one permission and allow another thread to run.
}

void downloadOneFile(ThreadInfo * threadInfo)
{
	WaitForSingleObject(semaphore, INFINITE);		//Wait for permission from Semaphore
	size_t numFile = 0;
	DWORD byteRead = 0;
	DWORD byteWrite = 0;
	DWORD totalByteRead = 0;
	DWORD status = 0;
	DWORD fileSize = 0;
	DWORD size = sizeof(DWORD);
	DWORD numberOfByte = KB;
	wstring newOutputDir = { 0 };
	HANDLE hFile = nullptr;
	HINTERNET hUrl = nullptr;

	if (!threadInfo) {
		return;
	}

	hUrl = InternetOpenUrlW(threadInfo->hInternet, threadInfo->url.data(), NULL, 0, INTERNET_FLAG_RELOAD, NULL);
	if (!hUrl)
	{
		updateStatusAndError(threadInfo, ErrorDownload::INTERNET_OPEN_ERROR, StatusDownload::ERROR_DOWNLOAD);
		writeToConsole(SHOW_THREAD, 0, threadInfo);
		CloseHandle(threadInfo->threadHandle);
		threadInfo->threadHandle = nullptr;
		return;
	}

	if (!HttpQueryInfoW(hUrl, HTTP_QUERY_STATUS_CODE | HTTP_QUERY_FLAG_NUMBER, &status, &size, NULL) ||
		!HttpQueryInfoW(hUrl, HTTP_QUERY_CONTENT_LENGTH | HTTP_QUERY_FLAG_NUMBER, &fileSize, &size, NULL))
	{
		if (status == HTTP_STATUS_NOT_FOUND) {
			updateStatusAndError(threadInfo, ErrorDownload::HTTP_404_CODE, StatusDownload::ERROR_DOWNLOAD);
		}
		else {
			updateStatusAndError(threadInfo, ErrorDownload::HTTP_ERROR, StatusDownload::ERROR_DOWNLOAD);
		}
		writeToConsole(SHOW_THREAD, 0, threadInfo);
		InternetCloseHandle(hUrl);
		CloseHandle(threadInfo->threadHandle);
		threadInfo->threadHandle = nullptr;
		return;
	}

	if (PathFileExistsW(threadInfo->filePath.data()) && threadInfo->overwrite == false) {
		do {
			numFile++;
			newOutputDir = threadInfo->filePath.substr(0, threadInfo->filePath.find_last_of(L'.')) + L"(" + to_wstring(numFile) + L")." + threadInfo->filePath.substr(threadInfo->filePath.find_last_of(L'.') + 1);
		} while (PathFileExistsW(newOutputDir.data()));
		threadInfo->filePath = newOutputDir.data();
	}

	WaitForSingleObject(mutex, INFINITE);			// Wait for a permission from the mutex
	hFile = CreateFileW(threadInfo->filePath.data(), GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	ReleaseMutex(mutex);							// Release one permission and allow another thread to run.

	if (hFile == INVALID_HANDLE_VALUE)
	{
		updateStatusAndError(threadInfo, ErrorDownload::CREATE_FILE_ERROR, StatusDownload::ERROR_DOWNLOAD);
		InternetCloseHandle(hUrl);
		CloseHandle(threadInfo->threadHandle);
		threadInfo->threadHandle = nullptr;
		return;
	}

	char* buffer = new char[KB];
	if (buffer == nullptr) {
		updateStatusAndError(threadInfo, ErrorDownload::NEW_BUFFER_FALSE, StatusDownload::ERROR_DOWNLOAD);
		return;
	}

	while (true)
	{
		updateStatusAndError(threadInfo, ErrorDownload::NONE, StatusDownload::DOWNLOADING);

		if (!InternetReadFile(hUrl, buffer, numberOfByte, &byteRead)) {
			updateStatusAndError(threadInfo, ErrorDownload::INTERNET_READ_FILE_FAILED, StatusDownload::ERROR_DOWNLOAD);
			break;
		}
		if (threadInfo->cancelEvent == true) {
			CloseHandle(hFile);
			updateStatusAndError(threadInfo, ErrorDownload::NONE, StatusDownload::CANCELLED);
			if (DeleteFileW(threadInfo->filePath.c_str()) == 0) {
				updateStatusAndError(threadInfo, ErrorDownload::DELETE_FILE_ERROR, StatusDownload::ERROR_DOWNLOAD);
			}
			break;
		}
		if (!WriteFile(hFile, buffer, byteRead, &byteWrite, NULL)) {
			updateStatusAndError(threadInfo, ErrorDownload::WRITE_FILE_ERROR, StatusDownload::ERROR_DOWNLOAD);
			break;
		}
		totalByteRead += byteRead;
		threadInfo->fileSize = totalByteRead;
		if (fileSize > 0) {
			threadInfo->progress = (int)(totalByteRead * 100LL / fileSize);
		}
		else {
			updateStatusAndError(threadInfo, ErrorDownload::FILE_EMPTY, StatusDownload::ERROR_DOWNLOAD);
			break;
		}
		writeToConsole(SHOW_THREAD, 0, threadInfo);
		if (threadInfo->progress == 100) {
			updateStatusAndError(threadInfo, ErrorDownload::NONE, StatusDownload::COMPLETED);
			break;
		}
	}

	InternetCloseHandle(hUrl);
	if (threadInfo->threadHandle != nullptr) {
		CloseHandle(threadInfo->threadHandle);
		threadInfo->threadHandle = nullptr;
	}
	delete[] buffer;

	WaitForSingleObject(mutex, INFINITE);		// Wait for a permission from the mutex
	totalDataDownloaded += totalByteRead;
	ReleaseMutex(mutex);						// Release one permission and allow another thread to run.
	ReleaseSemaphore(semaphore, 1, NULL);		// Release one permission and allow another thread to run.
}

void downloadMultipleFiles(vector<wstring> links, wstring outputDir, bool myOverwrite)
{
	system("cls");
	cout << "*--------Running thread list---------*" << endl;
	int dwMaxConnections = (int)links.size();
	size_t numberLinks = links.size();
	size_t sameNumberFile = 0;
	string choose;
	vector<ThreadInfo*> newThread;
	vector<HANDLE> thread;
	vector<wstring> downloadedFilenames;
	HINTERNET hInternet = InternetOpenW(NULL, INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0);

	if (hInternet != NULL)
	{
		InternetSetOptionW(NULL, INTERNET_OPTION_MAX_CONNS_PER_SERVER, &dwMaxConnections, sizeof(dwMaxConnections));
	}
	else {
		cout << "Failed to initialize Internet connection!" << endl;
		return;
	}

	semaphore = CreateSemaphore(NULL, MAXTHREADS, MAXTHREADS, NULL);		// Limit access thread
	if (!semaphore)
	{
		cerr << "Failed to create semaphore." << endl;
		return;
	}

	mutex = CreateMutex(NULL, FALSE, NULL);				// Limit access thread
	if (mutex == NULL) {
		cerr << "Failed to create mutex" << endl;
		return;
	}
	for (size_t i = 0; i < numberLinks; i++) {
		writeToConsole(SHOW_WAIT, i, nullptr);
		ThreadInfo* info = new ThreadInfo;
		if (info == nullptr) {
			cout << "Memory allocation failed." << endl;
			return;
		}
		wstring file = getFileNameFromUrl(links[i], i);

		if (find(downloadedFilenames.begin(), downloadedFilenames.end(), file) == downloadedFilenames.end()) {
			downloadedFilenames.push_back(file);
			info->numThread = i;
			info->url = links[i];
			info->status = START;
			info->cancelEvent = false;
			info->fileName = file;
		}
		else {
			sameNumberFile++;
			writeToConsole(SHOW_DUBLINK, i, nullptr);
			delete info;
			continue;
		}

		info->filePath = outputDir + L"\\" + file;
		info->overwrite = myOverwrite;
		info->progress = 0;
		info->fileSize = 0;
		info->error = NONE;
		info->hInternet = hInternet;
		info->threadMutex = CreateMutex(NULL, FALSE, NULL);				// Limit access thread
		if (info->threadMutex == NULL) {
			cerr << "Failed to create mutex." << endl;
			return;
		};
		info->threadHandle = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)downloadOneFile, info, 0, NULL);
		if (info->threadHandle == NULL) {
			wcout << "Failed to create thread for URL: " << info->url << endl;
			delete info;
			continue;
		}
		else {
			info->threadId = GetThreadId(info->threadHandle);
		}
		newThread.push_back(info);
		thread.push_back(info->threadHandle);
	}

	writeToConsole(SHOW_OPTION, numberLinks, nullptr);
	while (true) {
		cout << "Enter option: ";
		if (inputString(choose) == true) {
			if (toLowerCase(choose) == "show") {
				showThreadList(newThread, sameNumberFile);
			}
			else if (toLowerCase(choose) == "cancel") {
				cancelDownload(newThread, sameNumberFile);
			}
			else if (toLowerCase(choose) == "exit") {
				for (ThreadInfo* thread : newThread) {
					if (thread->threadHandle != nullptr) {
						CloseHandle(thread->threadHandle);
						thread->threadHandle = nullptr;
					}
				}
				cout << "Total data downloaded: " << totalDataDownloaded << " bytes" << endl;
				break;
			}
			else {
				cout << "Error: You entered the wrong option, re-enter..." << endl;
			}
		}
		else {
			cout << "Warning: You can only enter 1 option at a time, re-enter..." << endl;
		}
	}
	const HANDLE* handles = thread.data();
	WaitForMultipleObjects((DWORD)thread.size(), handles, TRUE, INFINITE);
	InternetCloseHandle(hInternet);

	for (ThreadInfo* thread : newThread) {
		delete thread;
	}

	newThread.clear();		//Clear pointer has delete in vector
	CloseHandle(mutex);
	CloseHandle(semaphore);
}

int wmain(int argc, const wchar_t* argv[])
{
	system("cls");
	if (argc < 3) {
		cout << "Syntax Error!" << endl;
		cout << "<Program> <link1> <link2>...<linkN> --output <directory> --overwrite" << endl;
		return 0;
	}
	wstring path;
	wchar_t envPath[MAX_PATH];
	DWORD pathLength = 0;
	bool overwrite = false;
	vector<wstring> links;
	wstring outputDir;
	globalConsoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
	if (globalConsoleHandle == INVALID_HANDLE_VALUE || globalConsoleHandle == NULL) {
		cout << "Error accessing standard output!" << endl;
		return 0;
	}

	for (int i = 1; i < argc; i++) {
		path = argv[i];
		if (path == L"--output") {
			pathLength = ExpandEnvironmentStringsW(argv[i + 1], envPath, MAX_PATH);
			outputDir = (pathLength != 0 && pathLength < MAX_PATH) ? envPath : argv[i + 1];
			i++;
		}
		else if (path == L"--overwrite") {
			overwrite = true;
			break;
		}
		else {
			links.push_back(path);
		}
	}
	if (links.empty() || outputDir.empty()) {
		cout << "Syntax Error! No links or output directory specified." << endl;
		return 0;
	}
	if (createNestedDir(outputDir) == false) {
		cout << "Directory path error! Check the link again." << endl;
		cout << "Make sure: path does not exceed 260 characters." << endl;
	}
	else {
		downloadMultipleFiles(links, outputDir, overwrite);
	}
	return 0;
}