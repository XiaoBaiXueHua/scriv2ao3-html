#include <iostream>
#include <iomanip>
#include <filesystem>
#include <string>
#include <vector>
#include <regex>
#include "subfiles/cleaner.h"
#include "subfiles/cleaner.cpp"

using namespace std;

int main() {
	sClean scriv;
	scriv.showEntries();
	scriv.explorer();
	scriv.showMaze();
	cout << "...hewwo????" << endl;
	scriv.makeDir();
	return 0;
}