#ifndef SCRIV_SANITIZER
#define SCRIV_SANITIZER
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <locale>
#include <regex>
#include <filesystem>
#include "./jkp_std.h"

using namespace std;

class sClean
{
public:
	sClean(){}; // default constructor
	~sClean()
	{
		raw.close();
		cleaned.close();
	}

	void showMaze();
	
	string currentPath();
	void makeDir();
	void explorer();
	void showEntries();
	void loop();

private:
	bool bulk{false};
	fstream raw, cleaned;						  // input/output streams
	filesystem::directory_entry currFile{"html"}; // current file, initialized to the html folder
	vector<filesystem::directory_entry> entries;  // the vector to help us navigate i guess! and also the entries thing for displaying the stuff
	vector<string> fileMaze;
};

#endif // end of SCRIV_SANITIZER