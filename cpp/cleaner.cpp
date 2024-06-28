#include <iostream>
#include <iomanip>
#include <filesystem>
#include "subfiles/cleaner.h"
#include "subfiles/cleaner.cpp"

using namespace std;

int main() {
	sClean scriv;
	scriv.showEntries();
	scriv.explorer();
	scriv.showMaze();
	return 0;
}