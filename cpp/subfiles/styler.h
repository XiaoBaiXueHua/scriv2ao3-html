#ifndef SCRIV_CSS
#define SCRIV_CSS
#ifndef CONVERTER_DEPENDENCIES
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
// #include <algorithm>
#include <cctype> // these were used in the trim example on stackoverflow
#include <locale> // these were used in the trim example on stackoverflow
// #include <iterator>
#include <regex>
#include <chrono>
// #include <cstdlib>
#endif
#include <filesystem>

using namespace std;

class sClean{
public:
	sClean(); //default constructor
	sClean(string);
	~sClean() { //destructor to clear n close the streams? really not sure if this is necessary
		raw.close();
		cleaned.close();
		vpt = nullptr;
		strPt = nullptr;
		delete vpt;
		delete strPt;
	};
	// functions for externally making rules
	// void ruleInit(vector<string>);
	void ruleInit(fstream &);
	// void valInit(vector<string>);
	void valInit(fstream &); 

	regex toRegEx(string);
	// regex toRegEx(string &);
	void findStyle();

	void sanitize();

	void nester();
	void unnest(string);
	// void unnest(string, bool);

	void findEl(string);
	void findEl(string, string); //overloading for when you want to find an element w/a particular param, like how <style> has "type=text/css" in it.
	void redirStream(fstream &, string, bool);
	void inputName(), setName(string);
	void setBatch(bool);
	void setiPath(string), setoPath(string), setFullPath(string), setType(string); //set the current path, name, n type of file being worked with
	void setFile(filesystem::directory_entry);
	void setRaw(string), setClean(string);
	// fstream getRaw(), getClean();
	string getFullPath(bool), getTmpPath();
	void resetLineNum();

	void gClean(); // for cleaning up google docs specifically

	//f'ns for pushing the important style rules into a vector
	void Detector(vector<vector<string>>&, string, string);
	void ruler(vector<string>& ,string); // function for pushing the correct css keys n vals back correctly
	void blockClean(string&);
	void spClean(string&);

	void unminify(); // unminifies an html file

	void templace(string);
	void templace(string, string);
	void templace(string&, string, string);

	virtual void executor(); // executes all the actions based on the filename provided by the string; the "virtual" makes it overridable in child classes

	static void open(fstream &, string, bool); // function for automatically making files n optionally appending text to them
	
	void loggy(string);
	void loggy(vector<string>&);
	void loggy(vector<vector<string>>&);
	// void loggy(cssRules);
	// string logTime();

	void reset();
private:
	int numLines{1}, bodLine{1}; //this really probably does not need to be private... maybe static though!
	bool foundEl{false}, styleSw{false}, bodySw{false}, tmpEl{false}, bulk{false}; //generic switch that flips around depending on whether we've found our currently searched-for el, and then perm switches for style n body
	string fname{""}, tmpname{""}, fipath{""}, fopath{""}, ftype{""}, fullPath{""}; //filepath n filename
	filesystem::directory_entry currFile;
	string temp{""}, el{""}; //i suspect i will probably want somewhere to temporarily hold strings when tossing them around
	// string quot{char(34)}, lt{char(60)}, gt{char(62)}, amp{char(38)}; //need this for, like. man idk why the regex doesn't take it as it is.
	fstream raw, cleaned; //the two streams for the input/output
	fstream logger; // stream for logger
	regex elStart, elEnd; //regexes for the starting and ending elements that get used n redeclared during loops
	vector<vector<string>>impSp, impP; //vectors for the important p's n sp's
	vector<string>*strPt; // pointer to work w/the rules vectors
	// vector<string>rls, vls; // rules n values to be set in main() for the styler to search for
	regex rls, vls; // for the thing. you know the thing
	vector<vector<string>>*vpt;
	vector<int>asciis {34, 38, 39, 60, 62}; //vector of asciis that need to be replaced manually bc thanks scrivener. currently covers straight quotes, ampersands, and gt/lt
};

#endif //end of SCRIV_CSS