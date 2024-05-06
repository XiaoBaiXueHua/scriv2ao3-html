

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

class cssRules{
	public: //what if i just made my vars public. what then.
	cssRules() {};
	cssRules(std::string el, std::string klass, std::string rule, std::string val) {
		rules = {el, klass, rule, val};
	}
	void ruler(std::string str, std::string elname, std::string rule) {
	rules.push_back(elname); //first push the element name, since that's necessary
	std::string pat = elname + "." + elname[0] + "\\d+";
	//std::string klass;
	std::regex klass(".(\\w|\\d)+");
	//std::string elKlass = std::regex_match(str, klass);
	std::smatch mitch;
	if (std::regex_match(str, mitch, klass)) {
		std::cout << "there is a matching class for this string. here is mitch[1].str(): " << mitch[1].str() << std::endl << std::endl;
	}
	//return vec;
	//vec.push_back()
	
	//okay so. what this should now do is find the class name (so, like, locate the elname + the period, and then iterate until it hits a whitespace)
	//and then, can do getline with ";" as its terminator, regexp each line for the rule name
}
	std::vector<std::string> getRules() {return rules;};
	private:
	std::vector<std::string> rules; //so these should have three members in each: the className, the rule name, and then the rule value. well. maybe four? one for the elname too.
};

class sClean{
public:
	sClean(); //default constructor
	//sClean(std::fstream &, std::fstream &); //constructor for the streams
	//sClean(std::fstream, std::fstream, std::string); //constructor for setting the default folder for the input filepath
	sClean(std::string);
	~sClean() { //destructor to clear n close the streams? really not sure if this is necessary
		//raw -> nullptr;
		// raw.clear();
		raw.close();
		// cleaned.clear();
		cleaned.close();
	};

	//void findStyle(std::fstream &, std::string);
	//void findStyle(std::string);
	void findStyle();
	//void findStyle(std::string); //uhhh... version where you just input the name of the file?

	void sanitize();

	void findEl(std::string);
	void findEl(std::string, std::string); //overloading for when you want to find an element w/a particular param, like how <style> has "type=text/css" in it.
	void redirStream(std::fstream &, std::string);
	void inputName();
	void setiPath(std::string), setoPath(std::string), setName(std::string), setType(std::string); //set the current path, name, n type of file being worked with
	void setRaw(std::string), setClean(std::string);
	std::string getFullPath(bool), getTmpPath(bool);
	void resetLineNum();

	//f'ns for pushing the important style rules into a vector
	void pDetect(std::vector<std::string>&, std::string);
	//void spDetect(std::vector<std::string>&, std::string);
	//void spDetect(std::vector<cssRules>&, std::string);
	void spDetect(std::vector<std::vector<std::string>>&, std::string);

	//void ruler(std::string, std::string, std::string);

	void executor(); // executes all the actions based on the filename provided by the string

	static void open(std::fstream &, std::string); //function for automatically making
	static void open(std::fstream &, std::string, bool); //version for appending txt to the file, like for the logger

	void loggy(std::string str) { // will try to make these friends later but for now. whatever i guess.
		sClean::open(logger, "log.txt", true);
		logger << str << std::endl;
	};
	void loggy(std::vector<std::string>& strs) { // will try to make these friends later but for now. whatever i guess.
		sClean::open(logger, "log.txt", true);
		for (auto str : strs) {
			logger << str << std::endl;
		}
	};

	friend std::string trim() {

	};
private:
	int numLines{1}; //this really probably does not need to be private... maybe static though!
	bool foundEl{false}, styleSw{false}, bodySw{false}, tmpEl{false}; //generic switch that flips around depending on whether we've found our currently searched-for el, and then perm switches for style n body
	// bool diffInPath{false};
	// std::string defaultInPath{"html"}; //default is folder called "html"
	std::string fname{""}, tmpname{""}, fipath{""}, fopath{""}, ftype{""}, fullPath{""}; //filepath n filename
	std::string temp{""}, el{""}; //i suspect i will probably want somewhere to temporarily hold strings when tossing them around
	std::fstream raw, cleaned; //the two streams for the input/output
	std::fstream logger; //static stream for logger
	//std::vector<std::string>impP, impSp; //vectors for the important p's n sp's
	std::regex elStart, elEnd; //regexes for the starting and ending elements that get used n redeclared during loops
	std::vector<std::string>impP;
	//std::vector<cssRules>impSp;
	std::vector<std::vector<std::string>>impSp;
};


// static class cleanPath{
// public:
// 	cleanPath(); //default constructor
// 	cleanPath(std::string, std::string, std::string);
// };

#endif //end of SCRIV_CSS