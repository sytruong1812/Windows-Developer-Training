#include <iostream>
#include <fstream>
using namespace std;

int main() {
    // Mở file để ghi
    fstream file("C:\\Users\\SY TRUONG\\Documents\\MicronSoftware\\Input\\1GB_file.txt", ios::in | ios::out | ios::binary);

    if (!file) {
        cout << "Khong the mo file.";
        return 1;
    }

    // Định vị tới vị trí 15000
    file.seekp(10);

    // Ghi chuỗi "Hello"
    file << "Hello";

    // Đóng file
    file.close();

    cout << "Ghi thanh cong." << endl;

    return 0;
}
