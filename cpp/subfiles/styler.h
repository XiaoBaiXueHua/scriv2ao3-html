

#ifndef SCRIV_CSS
#define SCRIV_CSS
#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
#include <chrono>

class sClean{
public:
	sClean(); //default constructor
	sClean(std::string);
	~sClean() { //destructor to clear n close the streams? really not sure if this is necessary
		//raw -> nullptr;
		// raw.clear();
		raw.close();
		// cleaned.clear();
		cleaned.close();
		vpt = nullptr;
		strPt = nullptr;
		delete vpt;
		delete strPt;
		// delete elStart;
	};

	//void findStyle(std::fstream &, std::string);
	//void findStyle(std::string);
	void findStyle();
	//void findStyle(std::string); //uhhh... version where you just input the name of the file?
	// void neet(std::string &, std::string); 

	void sanitize();

	void nester();
	void unnest(std::string);
	void unnest(std::string, bool);

	void findEl(std::string);
	void findEl(std::string, std::string); //overloading for when you want to find an element w/a particular param, like how <style> has "type=text/css" in it.
	void redirStream(std::fstream &, std::string, bool);
	void inputName();
	void setiPath(std::string), setoPath(std::string), setName(std::string), setType(std::string); //set the current path, name, n type of file being worked with
	void setRaw(std::string), setClean(std::string);
	std::string getFullPath(bool), getTmpPath();
	void resetLineNum();

	//f'ns for pushing the important style rules into a vector
	void Detector(std::vector<std::vector<std::string>>&, std::string, std::string);
	void ruler(std::vector<std::string>& ,std::string); // function for pushing the correct css keys n vals back correctly
	void blockClean(std::string&);
	void spClean(std::string&);

	void templace(std::string);
	void templace(std::string, std::string);
	void templace(std::string&, std::string, std::string);

	void executor(); // executes all the actions based on the filename provided by the string

	static void open(std::fstream &, std::string, bool); // function for automatically making files n optionally appending text to them

	void loggy(std::string);
	void loggy(std::vector<std::string>&);
	void loggy(std::vector<std::vector<std::string>>&);
	// void loggy(cssRules);
	// std::string logTime();

	void reset();
private:
	int numLines{1}; //this really probably does not need to be private... maybe static though!
	bool foundEl{false}, styleSw{false}, bodySw{false}, tmpEl{false}, bulk{false}; //generic switch that flips around depending on whether we've found our currently searched-for el, and then perm switches for style n body
	std::string fname{""}, tmpname{""}, fipath{""}, fopath{""}, ftype{""}, fullPath{""}; //filepath n filename
	std::string temp{""}, el{""}; //i suspect i will probably want somewhere to temporarily hold strings when tossing them around
	std::string quot{char(34)}, lt{char(60)}, gt{char(62)}, amp{char(38)}; //need this for, like. man idk why the regex doesn't take it as it is.
	std::fstream raw, cleaned; //the two streams for the input/output
	std::fstream logger; // stream for logger
	std::regex elStart, elEnd; //regexes for the starting and ending elements that get used n redeclared during loops
	std::vector<std::vector<std::string>>impSp, impP; //vectors for the important p's n sp's
	std::vector<std::string>*strPt; // pointer to work w/the rules vectors
	std::vector<std::vector<std::string>>*vpt;
};

#endif //end of SCRIV_CSS