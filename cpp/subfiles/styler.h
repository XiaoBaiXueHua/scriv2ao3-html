

#ifndef SCRIV_CSS
#define SCRIV_CSS
#include <iostream>
#include <iomanip>
#include <fstream>
#include <vector>
#include <string>
#include <iterator>
#include <regex>

class sClean{
public:
	sClean(); //default constructor
	//sClean(std::fstream &, std::fstream &); //constructor for the streams
	//sClean(std::fstream, std::fstream, std::string); //constructor for setting the default folder for the input filepath
	sClean(std::string);
	~sClean() { //destructor to clear n close the streams? really not sure if this is necessary
		//raw -> nullptr;
		raw.clear();
		raw.close();
		cleaned.clear();
		cleaned.close();
	};

	//void findStyle(std::fstream &, std::string);
	//void findStyle(std::string);
	void findStyle();
	//void findStyle(std::string); //uhhh... version where you just input the name of the file?

	void sanitize();

	std::string findEl(std::string, bool);
	static std::string findEl(std::string, std::string, bool); //overloading for when you want to find an element w/a particular param, like how <style> has "type=text/css" in it.
	void redirStream(std::fstream &, std::string);
	void inputName();
	void setiPath(std::string), setoPath(std::string), setName(std::string), setType(std::string); //set the current path, name, n type of file being worked with
	void setRaw(std::string), setClean(std::string);
	std::string getFullPath(bool);
	void resetLineNum();

	//f'ns for pushing the important style rules into a vector
	void pDetect(std::vector<std::string>&, std::string);
	void spDetect(std::vector<std::string>&, std::string);

	void ruler(std::string, std::string, std::string);

	void executor(std::string); // executes all the actions based on the filename provided by the string

	static void open(std::fstream &, std::string); //function for automatically making
private:
	int numLines{1}; //this really probably does not need to be private... maybe static though!
	// bool diffInPath{false};
	// std::string defaultInPath{"html"}; //default is folder called "html"
	std::string fname{""}, fipath{""}, fopath{""}, ftype{""}, fullPath{""}; //filepath n filename
	std::string temp{""}; //i suspect i will probably want somewhere to temporarily hold strings when tossing them around
	std::fstream raw, cleaned; //the two streams for the input/output
	std::vector<std::string>impP, impSp; //vectors for the important p's n sp's
};

class cssRules{
	public: //what if i just made my vars public. what then.
	std::vector<std::string> rule; //so these should have three members in each: the className, the rule name, and then the rule value. well. maybe four? one for the elname too.
	private:
};

// static class cleanPath{
// public:
// 	cleanPath(); //default constructor
// 	cleanPath(std::string, std::string, std::string);
// };

#endif //end of SCRIV_CSS