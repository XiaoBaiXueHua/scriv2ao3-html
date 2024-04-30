#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <iterator>
#include <regex>
#include "./styler.h"

sClean::sClean() {
	inputName();
	setiPath("html"); //default in path folder is html
	setoPath("output");
}
sClean::sClean(std::string path)
{
	inputName();
	setiPath(path);
	setoPath("output");
	
	// diffInPath = true;
	// defaultInPath = path;
}

//void sClean::findStyle(std::fstream &rawStream, std::string name)
//void sClean::findStyle(std::string name)
void sClean::findStyle()
{ // takes the raw stream bc it will open the cleaned stream w/in the function. also the raw stream should be an html file

	std::string line{""}; // initialize to blank
	//setName(name), 
	setType("html");
	// sClean::open(rawStream, getFullPath());
	setRaw(getFullPath(true));

	// std::fstream cleanStream;
	setType("css"); // yeah they're getting hard-coded
	setClean(getFullPath(false));
	// sClean::open(cleanStream, getFullPath());

	std::regex styleStart(sClean::findEl("style", "type=\"text/css\"", false));
	std::regex styleEnd(sClean::findEl("style", true));
	bool style{false};
	resetLineNum();
	// std::vector<std::string>* pointP = &impP;
	// std::vector<std::string>* pointSp = &impSp;
	// auto impP = new std::vector<std::string>; // pointer towards vector of important p
	// auto impSp = new std::vector<std::string>; // points to vector of important spans
	//std::string* impP{nullptr}; // initialize the pointer to nullptr
	//*impP -> impP;
	//impP = new std::vector<std::string>; //vector of important p

	while (std::getline(raw, line))
	{
		//std::cout << "Line " << numLines << ": " << line << std::endl;
		if (raw.eof() || std::regex_search(line, styleEnd))
		{ // if we reached the end of the file or </style>
			std::cout << "End of style found on line " << numLines << ".\n";
			break;
		}
		if (style) { 
			cleaned << line << std::endl; 
			if(std::regex_search(line, std::regex("p.p\\d+"))) {
				// std::cout << "This line dictates the styling for a paragraph: " << line << std::endl;
				pDetect(impP, line);
			} else if (std::regex_search(line, std::regex("span.(s\\d+|Scrivener-converted-space)"))) {
				spDetect(impSp, line);
				// std::cout << "This line styles a span styling: " << line << "\n";
			}
		}
		if (std::regex_search(line, styleStart)) { 
			style = true; 
			std::cout << "Start of style found on line " << numLines << ".\n";
			}
		numLines++;
	}
	std::cout << "The <style> element has now been copied over to a css file." << std::endl << std::endl;
	// std::cout << "We shall now look at the vectors of important elements.\n\n";
	// std::cout << "Starting with the pees: \n";
	// for (const auto rule : impP) {
	// 	std::cout << rule << "\n";
	// }
	// std::cout << "\nAnd now onto the spees:\n";
	// for (const auto rule : impSp) {
	// 	std::cout << rule << "\n";
	// }
	
	//DO NOT. FUCKIGN FORGET. TO DESTROY THY POINTERS.
	// pointP = nullptr;
	// pointSp = nullptr;
	// delete pointP;
	// delete pointSp;
	//impP -> nullptr;
	//peePoint = nullptr;
	//delete peePoint;
}

void sClean::sanitize() {
	// okay so. what this does is that it continues to iterate through the html file.
	// it ignores things until it hits the <body> element, after which it starts checking the element class names against its vector of allowed class names
	// for the p's, if it doesn't match any allowed classes, then just remove the class name. for the spans, remove the entire el if it doesn't fit. if it does, then for both, replace just the el + class w/the correct semantic name, and then replace the nearest closing sp/p with the correct closing el as well
	//when outputting the html this first time, don't have the endl, and keep it in a temp file, which will get deleted at the end.
	//the temp file is then read to further sanitize by checking for misnesting and the like all at once
}

void sClean::inputName() {
	std::string filename{""};
	std::cout << "Choose the name of the html file to open: ";
	std::cin >> filename;
	setName(filename);
}

void sClean::setiPath(std::string str) { fipath = str + "/"; } // will also automatically add in the /
void sClean::setoPath(std::string str) { fopath = str + "/"; }
void sClean::setName(std::string str) { fname = str; } // the name should only really be used once
void sClean::setType(std::string str) { ftype = "." + str; } // so in this case, Do Not include the period

//functions for redirecting the stream to other files
void sClean::redirStream(std::fstream &stm, std::string path)
{
	if (stm.is_open())
	{
		stm.clear();
		stm.close();
	}
	sClean::open(stm, path);
}
void sClean::setRaw(std::string path)
{
	redirStream(raw, path);
	// if (raw.is_open()) { //if it's already open, then clear n close it
	// 	raw.clear();
	// 	raw.close();
	// }
	// sClean::open(raw, path);
	// raw& operator= &r;
	// };
}
void sClean::setClean(std::string path)
{
	redirStream(cleaned, path);
	// if (cleaned.is_open()) {
	// 	cleaned.clear();
	// 	cleaned.close();
	// }
	// sClean::open(cleaned, path);
}

std::string sClean::getFullPath(bool instream) //true means it's the input path
{ // just to quickly concatenate that
std::string fpath = instream ? fipath : fopath;
	fullPath = fpath + fname + ftype;
	return fullPath;
}
void sClean::resetLineNum() { numLines = 1; } // should be set to 1 bc getline starts w/line 1

std::string sClean::findEl(std::string name, bool endpt)
{
	return (bool(endpt) ? "</" : "<") + name + ">";
}
std::string sClean::findEl(std::string name, std::string attributes, bool endpt)
{
	return (endpt ? "</" : "<") + name + " " + attributes + ">";
}

void sClean::pDetect(std::vector<std::string>& pee, std::string l) {
	if(std::regex_search(l, std::regex("(text-align|margin-left: \\d.\\d+)"))) {
		pee.push_back(l);
	}
}
void sClean::spDetect(std::vector<std::string>& sp, std::string l) {
	if (std::regex_search(l, std::regex("(text-decoration|white-space)"))) { 
		sp.push_back(l);
	}

}

void sClean::ruler(std::string str, std::string elname, std::string rule) {
	std::string pat = elname + "." + elname[0] + "\\d+";
	//std::string klass;
	std::regex klass(".(\\w|\\d)+");
	
	//okay so. what this should now do is find the class name (so, like, locate the elname + the period, and then iterate until it hits a whitespace)
	//and then, can do getline with ";" as its terminator, regexp each line for the rule name
}

void sClean::open(std::fstream &stream, std::string path)
{ // know that this is static
	if (!stream.is_open())
	{
		// std::cout << "the stream is not open. let us open it to " << path << std::endl;
		try
		{
			stream.open(path); // first check to make sure i didn't just forget to open it
			//std::cout << &stream;
			sClean::open(stream, path); //yeah sure. let's try recursion. i'm sure this won't go wrong.
		}
		catch (std::error_code)
		{
			// std::cout << "that did not work. let us make a new file i guess!\n";
			stream.clear();
			stream.open(path, std::ios::out); // if the file doesn't already exist, make it
			stream.close();
			stream.open(path);
		}
	}
}