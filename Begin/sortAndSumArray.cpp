#include <iostream>

#define LENGTH_CMD 8191

using namespace std;

void swapTwoNumbers(int* numberOne, int* numberTwo) {
	*numberOne = *numberOne ^ *numberTwo;
	*numberTwo = *numberOne ^ *numberTwo;
	*numberOne = *numberOne ^ *numberTwo;
}

unsigned long long sumOfArray(const int* arr, const int& num) {
	unsigned long long sum = 0;
	for (int i = 0; i < num; i++) {
		sum += *(arr + i);
	}
	return sum;
}

void sortElements(int* arr, int& num, bool option) {
	for (int i = 0; i < num; i++) {
		for (int j = i + 1; j < num; j++) {
			if (option ? (arr[i] > arr[j]) : arr[i] < arr[j]) {
				swapTwoNumbers(&arr[i], &arr[j]);
			}
		}
	}
}

void showEmlementSort(const int* arr, int& num) {
	cout << "Sort: ";
	for (int i = 0; i < num; i++) {
		cout << arr[i] << ' ';
	}
	cout << endl;
}

unsigned long long LengthSearch(const int* ptr) {
	unsigned long long count = 0;
	while (*ptr != '\0') {
		count++;
		ptr++;
	}
	return count;
}

bool inputOneNumber(int& num) {
	cin >> num;
	if (cin.fail() || num > numeric_limits<int>::max()) {
		cout << "Invalid input!" << endl;
		cin.clear();
		cin.ignore(LENGTH_CMD, '\n');
		return false;
	}
	else {
		return true;
	}
}

void sortAndSumNumberInArray(int* arr, int& num) {
	int again = 1;
	do {
		char choose;
		cout << "Ascending: A | Descending: D | Sum: S" << endl;
		cout << "Enter option: ";
		cin >> choose;
		switch (choose) {
		case 'A':
			sortElements(arr, num, 1);
			showEmlementSort(arr, num);
			break;
		case 'D':
			sortElements(arr, num, 0);
			showEmlementSort(arr, num);
			break;
		case 'S':
			cout << "The sum of the numbers in the array is: " << sumOfArray(arr, num) << endl;
			break;
		default:
			cout << "Invalid option, please try again." << endl;
			break;
		}
		bool againValid = false;
		while (!againValid) {
			cout << "Exit: 0 | Continue: 1" << endl;
			cout << "Enter number: ";
			againValid = inputOneNumber(again);
			if (again != 0 && again != 1) {
				cout << "Invalid option, please try again." << endl;
				againValid = false;
			}
		}
	} while (again);

}

int main() {
	int numberOfElements = 0;
	bool inputValid = false;
	while (!inputValid) {
		cout << "Enter the number size: ";
		inputValid = inputOneNumber(numberOfElements);

		if (inputValid && numberOfElements >= 2) {

			int* arr = new int[numberOfElements];

			for (int i = 0; i < numberOfElements; i++) {
				bool valid = false;
				while (!valid) {
					cout << "Enter element " << i + 1 << ": ";
					valid = inputOneNumber(arr[i]);
				}
			}
			sortAndSumNumberInArray(arr, numberOfElements);
			delete[] arr;
		}
		else {
			cout << "Note: numberOfElements must be greater than or equal to 2 elements." << endl;
			inputValid = false;
		}
	}
	return 0;
}