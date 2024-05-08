#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
#include <chrono>
#include "./styler.h"

using namespace std;

sClean::sClean()
{
	// inputName();
	setiPath("html"); // default in path folder is html
	setoPath("output");
}
sClean::sClean(string path)
{
	// inputName();
	setiPath(path);
	setoPath("output");

	// diffInPath = true;
	// defaultInPath = path;
}

// void sClean::findStyle(fstream &rawStream, string name)
// void sClean::findStyle(string name)

// i think later transfer the getline() loop to executor() so that it can iterate through the entire source file exactly one time
void sClean::findStyle()
{ // takes the raw stream bc it will open the cleaned stream w/in the function. also the raw stream should be an html file
	if (regex_search(temp, elEnd))
	{					// if we reached the end of the file or </style>
		styleSw = true; // sets the permanent switch for found style to true
		// lBreak = false;
		foundEl = false;			 // this gets declared inside the findStyle and sanitize() functions
		findEl("body");				 // okay so have to do it this way in order to make the css work correctly
		setType("html");			 // changes the next file type to html
		setClean(getTmpPath(false)); // switches the stream to the new tmp file
		loggy("the vector of important p's: ");
		loggy(impP);
		loggy("the vector of important spans: ");
		loggy(impSp);
	}
	if (foundEl)
	{
		smatch m;
		if (regex_search(temp, m, regex("(p|span|h\\d)."))) {
			string mstr = m.str().substr(0, m.str().length()-1); // pop off the .
			// auto vpt = new vector<vector<string>>; 
			if (mstr == "p") {
				vpt = &impP;
			} else if (mstr == "span") {
				vpt = &impSp;
			}
			Detector(*vpt, mstr, temp);
			vpt = nullptr;
			delete vpt;
		}
		// if (regex_search(temp, regex("p.p\\d+")))
		// {
		// 	// pDetect(impP, temp);
		// }
		// else if (regex_search(temp, regex("span.(s\\d+|Scrivener-converted-space)")))
		// {
		// 	// spDetect(impSp, temp);
		// 	Detector(impSp, "span", temp);
		// }
		cleaned << temp << endl;
	}
}

void sClean::sanitize()
{
	// while (getline(raw, temp)) {
	// if (raw.eof() || regex_search(temp, elEnd)) {
	// 	cout << "reached the end of the file or smth idk." << endl;
	// }
	// if (foundEl) {
	// 	//  temp = regex_replace(temp, regex("&#34;"), "\"");
	// 	//  cleaned << temp << endl; // later remove the endl so that we can have another function clean up the misnesting, but for now just print it so we can seeeee it
	// }
	if (regex_search(temp, elEnd)) { 
		foundEl = false; 
		bodySw = true; 
		setClean(getFullPath(false));
		}
	if (foundEl)
	{
		// regex klass("class=\".+\"");
		bool cleanP{false};
		string anyClass{"class=\"(\\w|\\d)+\""};
		int i{0};
		// auto ruleVec = new vector<string>; //pointer for the correct vector
		while (i < impP.size()) {
			if (regex_search(temp, regex("class=\""+impP[i][1]+"\""))) {
				cout << "this paragraph is supposed to have a replacement done. behold the raw:\n\t" << temp << endl;
				cleanP = true;
				strPt = &(impP[i]);
				// ruleVec = &impP[i];
				break;
			}
			i++; // you fool. do not forget to iterate
		}
		// for (auto p : impP) {
		// 	if (regex_search(temp, regex("class=\""+p[1]+"\""))) {
		// 		cout << "this paragraph is supposed to have a replacement done. behold the raw:\n\t" << temp << endl;
		// 		cleanP = true;
		// 		break;
		// 	}
		// }
		if (cleanP) {
			// string rule = *strPt[2];
			// cout << "strPt[2] rule: " << rule << endl;
			// cout << "&string pointer [2]: " << *(strPt[2]) << endl;
			// rule = nullptr;
		} else {
			temp = regex_replace(temp, regex("<p "+anyClass+">"), "<p>"); // otherwise just replace the class
		}
		// so each paragraph is in temp, so
		cleaned << temp;

		//and then at the end, delete
		// ruleVec = nullptr;
		// delete ruleVec;
	}
	if (regex_search(temp, elStart))
	{
		foundEl = true;
	}

	// }
	// okay so. what this does is that it continues to iterate through the html file.
	// it ignores things until it hits the <body> element, after which it starts checking the element class names against its vector of allowed class names
	// for the p's, if it doesn't match any allowed classes, then just remove the class name. for the spans, remove the entire el if it doesn't fit. if it does, then for both, replace just the el + class w/the correct semantic name, and then replace the nearest closing sp/p with the correct closing el as well
	// when outputting the html this first time, don't have the endl, and keep it in a temp file, which will get deleted at the end.
	// the temp file is then read to further sanitize by checking for misnesting and the like all at once
}

void sClean::inputName()
{
	string filename{""};
	cout << "Choose the name of the html file to open: ";
	cin >> filename;
	setName(filename);
}

void sClean::setiPath(string str) { fipath = str + "/"; } // will also automatically add in the /
void sClean::setoPath(string str) { fopath = str + "/"; }
void sClean::setName(string str)
{
	fname = str;
	tmpname = str + "_tmp";
} // the name should only really be used once
void sClean::setType(string str) { ftype = "." + str; } // so in this case, Do Not include the period

// functions for redirecting the stream to other files
void sClean::redirStream(fstream &stm, string path)
{
	if (stm.is_open()) // if there's a file already open
	{
		// stm.clear();
		stm.close(); // then just close it
	}
	sClean::open(stm, path); // and then do the standard opening procedure
							 // loggy("the path has been directed to "+path);
}
void sClean::setRaw(string path)
{
	// loggy("the raw stream path has been set to: " + path);
	redirStream(raw, path);
}
void sClean::setClean(string path)
{
	// loggy("the output path has been set to: " + path);
	redirStream(cleaned, path);
}

string sClean::getFullPath(bool instream) // true means it's the input path
{										  // just to quickly concatenate that
	string fpath = instream ? fipath : fopath;
	fullPath = fpath + fname + ftype;
	// loggy("full path: "+fullPath);
	return fullPath;
}

string sClean::getTmpPath(bool instream)
{
	string fpath = instream ? fipath : fopath;
	fullPath = fpath + tmpname + ftype;
	// loggy("tmp path: "+fullPath);
	return fullPath;
}

void sClean::resetLineNum() { numLines = 1; } // should be set to 1 bc getline starts w/line 1

void sClean::findEl(string name)
{
	el = name;
	elStart = "<" + el + ">";
	elEnd = "</" + el + ">";
	// loggy(vector<string>{"looking for: " + name, "with regexp start: <" + el + "> and regexp end: </" + el + ">"});
}
void sClean::findEl(string name, string attributes)
{
	el = name;
	elStart = "<" + el + " " + attributes + ">";
	elEnd = "</" + el + ">";
	// loggy(vector<string>{"looking for: " + name, "with regexp start: <" + el + " " + attributes +  "> and regexp end: </" + el + ">"});
}


void sClean::Detector(vector<vector<string>>&els, string elm, string l) {
	regex rule("(text-align|margin-left|text-decoration|white-space)"); // this covers both p and sp and yeah it's getting hard-coded for now
	if (regex_search(l, rule)) {

		// cout << "this is a relevant " << elm << " rule! here's the full string we'll be looking at:\n" << l << "\n\n";
		vector<string> r;
		r.push_back(elm);
		const regex klass(elm+".(\\S)+");
		smatch mitch;
		if (regex_search(l, mitch, klass)) {
			r.push_back(mitch.str().substr(elm.length()+1, mitch.str().length())); // should return just the class name
		}
		l = regex_replace(l, regex("(\\{|\\}|"+mitch.str()+"|\\s{2,})"), ""); //clean off the curly brackets, as well as the class substring and 2+ spaces
		string temp2;
		istringstream line;
		line.str(l);

		while (getline(line, temp2, ';')) {
			if (regex_search(temp2, mitch, rule)) {
				// string mstr = mitch.str();
				if (mitch.str() == "margin-left") {
					if (regex_search(mitch.str(), regex("\\d.\\d+"))) { //have to make sure this is, like. a proper blockquote el
						// r.push_back(temp2); // since we should've cut off the curly brackets, go ahead and push the whole thing back
						ruler(r, temp2);
					}
				} else {
					// r.push_back(temp2);
					ruler(r, temp2);
				}
			}
		}
		if (r.size() > 2) { //have to have at least 2 rules in order to be worth pushing back
			els.push_back(r); // and now finally. push the vector back into the master vector. though could probably also add in an additional clause to make sure it doesn't already match smth, that'll probably just overcomplicate things in the end
		}
	}
}
void sClean::ruler(vector<string>&v, string str) { //because it seems we cannot have local functions in c++ :pepehands:
	// so basically, we need to find the position of the ':' using regexp. we cut the rule off there. 
	string t;
	istringstream s;
	s.str(str);
	do { //have to do a do-while in order for it to pick up the whole thing for some reason. who knows why!
		if (!t.empty()) { // however, in doing so, we must also make sure that we don't push back the first empty string
			v.push_back(t);
		}
	} while (getline(s, t, ':'));
	// while (getline(s, t, ':')); { //wait i'm stupid we can just do getline
	// 	v.push_back(t);
	// }
}

void sClean::open(fstream &stream, string path)
{						   // know that this is static
	if (!stream.is_open()) // if for some reason it failed to open, or i just forgot
	{
		stream.open(path);
		if (!stream.is_open())
		{ // this had originally been recursive but for some reason it broke. not like it should've gone through more than two layers i guess
			// stream.close();
			stream.clear();
			stream.open(path, ios::out);
			stream.close();
			stream.open(path);
		}
	}
}

void sClean::open(fstream &stream, string path, bool append)
{
	if (!stream.is_open())
	{
		stream.open(path, ios_base::app);
		if (!stream.is_open())
		{
			stream.clear();
			stream.open(path, ios::out);
			stream.close();
			stream.open(path, ios_base::app);
		}
	}
}

void sClean::loggy(string str)
{ // will try to make these friends later but for now. whatever i guess.
	sClean::open(logger, "log.txt", true);
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());

	logger << put_time(localtime(&now), "[%F %T]: ") << str << std::endl;
}
void sClean::loggy(vector<string> &strs)
{ // will try to make these friends later but for now. whatever i guess.
	sClean::open(logger, "log.txt", true);
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	logger << put_time(localtime(&now), "[%F %T]:\n");
	for (auto str : strs)
	{
		logger << str << std::endl;
	}
}

void sClean::loggy(vector<vector<string>> &strs)
{
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	logger << put_time(localtime(&now), "[%F %T]:\n");
	for (int i{0}; i < strs.size(); i++)
	{
		logger << "Layer " << i << " of the 2D vector:\n";
		for (auto str : strs[i])
		{
			logger << str << " | ";
		}
		logger << endl;
	}
}

void sClean::reset()
{ // sets all the els back to false and closes all the streams
	numLines = 1;
	foundEl = false;
	styleSw = false;
	bodySw = false;
	tmpEl = false;
	raw.close();
	cleaned.close();
	impP = {{}};
	impSp = {{}};
	//set the pointers to null just in case
	strPt = nullptr;
	vpt = nullptr;
}

void sClean::executor()
{				 // so how this should probably work to keep everything inside one while loop is that the major functions like findStyle() and stuff, when they're done running, they should basically switch the findEl w/in them, rather than have it happen here in the executor
	inputName(); // input the name
	// loggy("\nNow logging for: " + fname + "\n");
	setType("html");
	// sClean::open(rawStream, getFullPath());
	setRaw(getFullPath(true)); // these automatically open the streams

	// fstream cleanStream;
	setType("css");				  // yeah they're getting hard-coded
	setClean(getFullPath(false)); // automatically opens the stream
	// sClean::open(cleanStream, getFullPath());
	resetLineNum(); // reset the current line number
	// el = "style";
	findEl("style", "type=\"text/css\""); // sets the private regexps for el start n end, in this case for the styling

	while (getline(raw, temp))
	{
		if (raw.eof())
		{
			cout << "\nEnd of file.\nOutput may be found in " << getFullPath(false) << ".\n";
			// loggy("\nEnd of file.\nOutput may be found in " + getFullPath(false) + ".\n");
			// foundEl = false; //set this to false for subsequent runs it seems
			reset();
			break;
		}
		if (regex_search(temp, elEnd))
		{
			// loggy("\nEnd of " + el + " found on line " + to_string(numLines) + ".\n");
			cout << "\nEnd of " << el << " found on line " << numLines << ".\n";
			// foundEl = false;
		}
		if (foundEl)
		{
			if (!styleSw)
			{
				findStyle(); // if the style switch is off, then do findStyle();
			}
			else if (!bodySw)
			{
				sanitize(); // otherwise, we're sanitizing the body
			}

			// cleaned << temp << endl; //the findStyle() and sanitize() f'ns are pretty much just to transform temp into smth clean
			// cleaned << temp << (lBreak ? "\n" : "");
		}
		if (regex_search(temp, elStart))
		{
			foundEl = true;
			cout << "Start of " << el << " found on line " << numLines << ".\n";
			// loggy("Start of " + el + " found on line " + to_string(numLines) + ".\n");
		}
		numLines++;
	}

	reset();
	// loggy("End of logging for: " + fname + "\n");
}