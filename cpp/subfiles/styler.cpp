#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <iterator>
#include <regex>
#include "./styler.h"

sClean::sClean() {
	setiPath("html"); //default in path folder is html
	setoPath("output");
}
sClean::sClean(std::string path)
{
	setiPath(path);
	setoPath("output");
	
	// diffInPath = true;
	// defaultInPath = path;
}

//void sClean::findStyle(std::fstream &rawStream, std::string name)
void sClean::findStyle(std::string name)
{ // takes the raw stream bc it will open the cleaned stream w/in the function. also the raw stream should be an html file

	std::string line{""}; // initialize to blank
	setName(name), setType("html");
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
	while (std::getline(raw, line))
	{
		std::cout << "Line " << numLines << ": " << line << std::endl;
		if (raw.eof() || std::regex_search(line, styleEnd))
		{ // if we reached the end of the file or </style>
			std::cout << "End of style found on line " << numLines << ".\n";
			break;
		}
		if (style) { 
			cleaned << line << std::endl; 
		}
		if (std::regex_search(line, styleStart)) { 
			style = true; 
			std::cout << "Start of style found on line " << numLines << ".\n";
			}
		numLines++;
	}
	std::cout << "The <style> element has now been copied over to a css file." << std::endl << std::endl;
}

void sClean::setiPath(std::string str) { fipath = str + "/"; } // will also automatically add in the /
void sClean::setoPath(std::string str) { fopath = str + "/"; }
void sClean::setName(std::string str) { fname = str; }
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
	std::string el{(bool(endpt) ? "</" : "<") + name + ">"};
	//std::cout << "w/o attributes, looking for: " << el << std::endl;
	return el;
}
std::string sClean::findEl(std::string name, std::string attributes, bool endpt)
{
	std::string el{(endpt ? "</" : "<") + name + " " + attributes + ">"};
	return el;
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