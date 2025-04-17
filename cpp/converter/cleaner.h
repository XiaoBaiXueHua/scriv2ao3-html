#ifndef MAIDS
#define MAIDS

#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cmath>
#include "options.h"
#include "whatever.h" // has cssRule and sanitize
// #include "whatever.cpp"
#include "li-ruby.h"
// #include "li-ruby.cpp"
#include "friends.h"
// #include "friends.cpp"

using namespace std;

class cleaner
{
public:
	cleaner() {}; // default constructor
	cleaner(filesystem::directory_entry);
	~cleaner(); // destructor

	void whoosh(); // main cleaning function
	void snipe();
	bool incl(string);

private:
	string tmp{""}, tmp2{""}, tmp3{""};
	string full{""}, guts{""};

	stringstream cleanLine;
	sanitize pls;
	cssRule relevant;
	fstream raw, cleaned, copier; // input/output streams + a copier to keep a copy of the original exported html n subsequently delete the original from the html-to-process folder
	stringstream sstr, sstr2;
	filesystem::directory_entry currFile{options::htmlFolder}; // current file, initialized to the html folder

	smatch smidge;
	bool listSwitch{false}, tableSwitch{false};
	long long unsigned int trimAway{0};


	void pushLine(vector<sanitize> &);		 // both pls && cleanLine are global, so this doesn't need to take those as args
	void pushLine(vector<sanitize> &, bool); // conditional version
	void pushLine(sanitize &, vector<sanitize> &, bool);
	
	sanitize *sanPtr = new sanitize;

protected:
	string innerHTML{""};
};

#endif