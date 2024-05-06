#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
// #include <chrono>
//#include <ctime>
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"

using namespace std;

int main()
{
	//chrono::system_clock aa;
	// time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now()); // https://en.cppreference.com/w/cpp/chrono/time_point
	// //const std::time_t t_c = std::chrono::system_clock::to_time_t(now - 24h);
	// //const auto t_c = chrono::system_clock::to_time_t(now - 24h);
	// // string now= to_string(chrono::system_clock::now());
	// cout << "The current time is: [" << put_time(localtime(&now), "%F %T.]\n") << endl;
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
			// scriv.inputName();
		}
	}
	
	
	// cout << "The script finished at: [" << put_time(localtime(&now), "%F %T.]\n");
	return 0;
}
