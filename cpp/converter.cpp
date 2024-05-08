#include <iostream>
#include <iomanip>
// #include <fstream>
#include <string>
// #include <vector>
// #include <algorithm>
// #include <iterator>
// #include <regex>
// #include <chrono>
//#include <ctime>
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"

using namespace std;

int main()
{
	char ans{'y'}, changeDir{'n'};
	string dir{""};

	sClean scriv;

	while (toupper(ans) == 'Y') {
		//scriv.findStyle();
		scriv.executor();
		cout << "Enter Y/y to convert another file, or else hit any key to exit: ";
		cin >> ans;

		if (toupper(ans) == 'Y') {
			cout << "\n\nWould you like to change the source folder for the html file? ";
			cin >> changeDir;
			if (toupper(changeDir) == 'Y') {
				cout << "\nPlease specify the name of the file folder: ";
				cin >> dir;
				scriv.setiPath(dir);
				cout << endl;
			}
		}
	}
	return 0;
}
