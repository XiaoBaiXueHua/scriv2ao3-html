#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include <iterator>
#include <regex>
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

	// string line{""}; // initialize to blank
	// setName(name),
	

	// regex styleStart(sClean::findEl("style", "type=\"text/css\"", false));
	// regex styleEnd(sClean::findEl("style", true));
	//bool style{false};
	// vector<string>* pointP = &impP;
	// vector<string>* pointSp = &impSp;
	// auto impP = new vector<string>; // pointer towards vector of important p
	// auto impSp = new vector<string>; // points to vector of important spans
	// string* impP{nullptr}; // initialize the pointer to nullptr
	//*impP -> impP;
	// impP = new vector<string>; //vector of important p
	
	
	//findEl("style", true);
	// while (getline(raw, temp))
	// {
		// cout << "Line " << numLines << ": " << line << endl;
		// if (raw.eof() || regex_search(temp, elEnd))
		if (regex_search(temp, elEnd))
		{ // if we reached the end of the file or </style>
			// cout << "End of " << el << " found on line " << numLines << ".\n";
			// break;
			styleSw = true; // sets the permanent switch for found style to true 
			foundEl = false; // this gets declared inside the findStyle and sanitize() functions
			// el = "body";
			findEl("body"); //okay so have to do it this way in order to make the css work correctly
			setType("html"); //changes the next file type to html
			setClean(getTmpPath(false)); // switches the stream to the new tmp file
			loggy("the vector of important p's: ");
			loggy(impP);
			// log("the vector of important spans: ");
			// log(impSp);
		}
		if (foundEl)
		{
			// line = line.trim();
			// cleaned << temp << endl;
			if (regex_search(temp, regex("p.p\\d+")))
			{
				// cout << "This line dictates the styling for a paragraph: " << line << endl;
				pDetect(impP, temp);
			}
			else if (regex_search(temp, regex("span.(s\\d+|Scrivener-converted-space)")))
			{
				spDetect(impSp, temp);
				//cout << "This line styles a span styling: " << line << "\n";
			}
		}
		// if (regex_search(temp, elStart))
		// {
		// 	foundEl = true; 
		// 	cout << "Start of " << el << " found on line " << numLines << ".\n";
		// }
		// numLines++;
	// }
	// cout << "The <style> element has now been copied over to a css file." << endl << endl;
	// cout << "We shall now look at the vectors of important elements.\n\n";
	// cout << "Starting with the pees: \n";
	// for (const auto rule : impP) {
	// 	cout << rule << "\n";
	// }
	// cout << "\nAnd now onto the spees:\n";
	// for (const auto rule : impSp) {
	// 	cout << rule << "\n";
	// }

	// DO NOT. FUCKIGN FORGET. TO DESTROY THY POINTERS.
	//  pointP = nullptr;
	//  pointSp = nullptr;
	//  delete pointP;
	//  delete pointSp;
	// impP -> nullptr;
	// peePoint = nullptr;
	// delete peePoint;
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
		if (regex_search(temp, elStart)) {
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
void sClean::setName(string str) { fname = str; tmpname = str + "_tmp";}		 // the name should only really be used once
void sClean::setType(string str) { ftype = "." + str; } // so in this case, Do Not include the period

// functions for redirecting the stream to other files
void sClean::redirStream(fstream &stm, string path)
{
	if (stm.is_open()) //if there's a file already open
	{
		// stm.clear();
		stm.close(); //then just close it
	}
	sClean::open(stm, path); //and then do the standard opening procedure
}
void sClean::setRaw(string path)
{
	redirStream(raw, path);
}
void sClean::setClean(string path)
{
	redirStream(cleaned, path);
}

string sClean::getFullPath(bool instream) // true means it's the input path
{											   // just to quickly concatenate that
	string fpath = instream ? fipath : fopath;
	fullPath = fpath + fname + ftype;
	return fullPath;
}

string sClean::getTmpPath(bool instream) {
	string fpath = instream ? fipath : fopath;
	fullPath = fpath + tmpname + ftype;
	return fullPath;
}

void sClean::resetLineNum() { numLines = 1; } // should be set to 1 bc getline starts w/line 1

void sClean::findEl(string name)
{
	el = name;
	elStart = "<" + el + ">";
	elEnd = "</" + el + ">";
	//return (endpt ? "</" : "<") + name + ">";
}
void sClean::findEl(string name, string attributes)
{
	el = name;
	elStart = "<" + el + " " + attributes + ">";
	elEnd = "</" + el + ">";
	//return (endpt ? "</" : "<") + name + " " + attributes + ">";
}

void sClean::pDetect(vector<string> &pee, string l)
{
	if (regex_search(l, regex("(text-align|margin-left: \\d.\\d+)")))
	{
		pee.push_back(l);
	}
}
//void sClean::spDetect(vector<string> &sp, string l)
void sClean::spDetect(vector<vector<string>> &sp, string l)
{
	// cout << "this rule has a match! here's the full string we'll be looking at: \n\n" << l << "\n\n";
	string rule{"(text-decoration|white-space)"};
	if (regex_search(l, regex(rule)))
	{
		// sp.push_back(l);
		cssRules r;
		r.ruler(l, "span", rule);
		//sp.push_back(r.rules);
		//auto a = r.rules;
		sp.push_back(r.getRules());
	}
}

void sClean::open(fstream &stream, string path)
{ // know that this is static
// bool tried{false};
// bool created{false};
	if (!stream.is_open()) //if for some reason it failed to open, or i just forgot
	{
		stream.open(path);
		if (!stream.is_open()) { //this had originally been recursive but for some reason it broke. not like it should've gone through more than two layers i guess
			// stream.close();
			stream.clear();
			stream.open(path, ios::out);
			stream.close();
			stream.open(path);
		}
		// try {
		// 	stream.open(path);
		// 	sClean::open(stream, path);
		// } catch (error_code) {
		// 	stream.close();
		// 	//stream.clear();
		// 	stream.open(path, ios::out);
		// 	stream.close();
		// 	stream.open(path);
		
		// try
		// {
		// 	stream.open(path); // first check to make sure i didn't just forget to open it
		// 	cout << "\naddress of the file stream: " << &stream << "\t\twe are trying to open: " << path;
		// 	bool aaa{stream.is_open()};
		// 	if (!aaa) {
		// 		cout << "\nit did not open. let us create it then.";
		// 		stream.clear();
		// 		stream.open(path, ios::out);
		// 		cout << "\nit should have been made now!";
		// 		stream.close();
		// 		stream.open(path);
		// 	}
		// 	// sClean::open(stream, path); // yeah sure. let's try recursion. i'm sure this won't go wrong.
		// 	// sClean::open(stream, path); // yeah sure. let's try recursion. i'm sure this won't go wrong.
		// 	//sClean::openLoops++;
		// }
		// //catch (error_code e)
		// catch(exception)
		// {
		// 	// cout << "that did not work. let us make a new file i guess!\n";
		// 	stream.clear();
		// 	cout << "\nwell that didn't work." << endl;
		// 	stream.open(path, ios::out); // if the file doesn't already exist, make it
		// 	stream.close();
		// 	stream.open(path);
		// }
	}
}

void sClean::open(fstream &stream, string path, bool append) {
	if (!stream.is_open()) {
		stream.open(path, ios_base::app);
		if (!stream.is_open()) {
			stream.clear();
			stream.open(path, ios::out);
			stream.close();
			stream.open(path, ios_base::app);
		}
	}
}

void sClean::executor() { //so how this should probably work to keep everything inside one while loop is that the major functions like findStyle() and stuff, when they're done running, they should basically switch the findEl w/in them, rather than have it happen here in the executor
	inputName(); //input the name

	setType("html");
	// sClean::open(rawStream, getFullPath());
	setRaw(getFullPath(true)); //these automatically open the streams

	// fstream cleanStream;
	setType("css"); // yeah they're getting hard-coded
	setClean(getFullPath(false)); // automatically opens the stream
	// sClean::open(cleanStream, getFullPath());
	resetLineNum(); //reset the current line number
	//el = "style";
	findEl("style", "type=\"text/css\""); //sets the private regexps for el start n end, in this case for the styling

	while (getline(raw, temp)) {
		if (raw.eof()) {
			cout << "\nEnd of file.\nOutput may be found in " << getFullPath(false) << ".\n";
			foundEl = false; //set this to false for subsequent runs it seems
			break;
		}
		if (regex_search(temp, elEnd)) {
			cout << "\nEnd of " << el << " found on line " << numLines << ".\n";
		}
		if (foundEl) {
			// switch(el) {
			// 	case "style": {}

			// }
			if (!styleSw) {
				findStyle(); // if the style switch is off, then do findStyle();
			} else if (!bodySw) {
				sanitize(); // otherwise, we're sanitizing the body
			}
			
			// cleaned << temp << endl; //the findStyle() and sanitize() f'ns are pretty much just to transform temp into smth clean
			cleaned << temp;
		}
		if (regex_search(temp, elStart)) {
			foundEl = true;
			cout << "Start of " << el << " found on line " << numLines << ".\n";
		}
		numLines++;
	}

	// findStyle();

	// setType("html"); //set the type to html for the sanitizer output
	// // setName(fname + "_temp"); //change the name
	// setClean(getFullPath(false)); //redirect
	// foundEl = false; //reset this for the sanitizer lol 
	// findEl("body");
	// sanitize();
}