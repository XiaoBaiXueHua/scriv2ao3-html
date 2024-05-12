#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype> // these were used in the trim example on stackoverflow
#include <locale> // these were used in the trim example on stackoverflow
#include <iterator>
#include <regex>
#include <chrono>
#include "./styler.h"

using namespace std;

// bad practice global var bc im Do Not Believe it needs to be a private member. there's enough of these.
string anyClass{"class=\"(\\w|\\d|-)+\""};

// trims from https://stackoverflow.com/216823/how-to-trim-a-stdstring
inline void ltrim(string &s)
{
	s.erase(s.begin(), find_if(s.begin(), s.end(), [](unsigned char ch)
							   { return !isspace(ch); }));
};
inline void rtrim(string &s)
{
	s.erase(find_if(s.rbegin(), s.rend(), [](unsigned char ch)
					{ return !isspace(ch); })
				.base(),
			s.end());
}
inline void vtrim(string &s)
{
	rtrim(s);
	ltrim(s);
}
string trim(string &s)
{
	vtrim(s);
	return s;
}

sClean::sClean()
{
	// inputName();
	setiPath("html"); // default in path folder is html
	setoPath("output");
}
sClean::sClean(string path)
{
	setiPath(path);
	setoPath("output");
}

void sClean::findStyle()
{ // takes the raw stream bc it will open the cleaned stream w/in the function. also the raw stream should be an html file
	if (regex_search(temp, elEnd))
	{					 // if we reached the end of the file or </style>
		styleSw = true;	 // sets the permanent switch for found style to true
		foundEl = false; // this gets declared inside the findStyle and sanitize() functions
		findEl("body");	 // okay so have to do it this way in order to make the css work correctly
		loggy("findEl now searching for the body.");
		setType("html");			 // changes the next file type to html
		setClean(getTmpPath()); // switches the stream to the new tmp file
		loggy("the vector of important p's: ");
		loggy(impP);
		loggy("the vector of important spans: ");
		loggy(impSp);
	}
	if (foundEl)
	{
		smatch m;
		if (regex_search(temp, m, regex("(p|span|h\\d).")))
		{
			string mstr = m.str().substr(0, m.str().length() - 1); // pop off the .
			// auto vpt = new vector<vector<string>>;
			if (mstr == "p" || regex_match(mstr, regex("h\\d"))) // will now also include headers
			{
				vpt = &impP;
			}
			else if (mstr == "span")
			{
				vpt = &impSp;
			}
			Detector(*vpt, mstr, temp);
			// vpt = nullptr;
			// delete vpt;
		}
		cleaned << temp << endl;
	}
}

void sClean::sanitize()
{
	if (regex_search(temp, elEnd))
	{
		foundEl = false;
		bodySw = true;
		// setRaw(getFullPath(true));
		// setClean(getFullPath(false));
	}
	if (foundEl)
	{
		blockClean(temp); // first clean the block
		spClean(temp);	  // then clean the in-lines
		// so each paragraph is in temp, so
		temp = regex_replace(temp, regex("\\s?"+anyClass), ""); // clear out all remaining class 
		// templace("\\&#34;", to_string(toascii(char(34))));
		temp = regex_replace(temp, regex("&#34;"), quot);
		temp = regex_replace(temp, regex("&#60;"), lt);
		temp = regex_replace(temp, regex("&#62;"), gt);
		unnest("em");
		unnest("strong");
		cleaned << temp;
	}
}

void sClean::nester()
{
	while (getline(raw, temp)) {
		unnest("blockquote", true);
		// cout << "Now unnesting the string:\n";
		// cout << temp << endl;
		cleaned << temp;
	}
	
	// there has got to be a way to do this efficiently even w/large files... since we're working w/streams here, then maybe do peek during the sanitize step of the blockClean()
	
}

void sClean::unnest(string elm) {
	templace("</"+elm+"><"+elm+">");
}
void sClean::unnest(string elm, bool nl) {
	// string clean{"</"+elm+"><"+elm+">"};
	templace("</"+elm+"><"+elm+">", (nl ? "\n" : ""));
}

// surely doing things like this will not go wrong at all
void sClean::templace(string &str, string exp, string rep) {
	// regex r(R"("+exp)");
	str = regex_replace(str, regex(exp), rep);
}
void sClean::templace(string exp, string rep) {
	templace(temp, exp, rep);
}
void sClean::templace(string exp) {
	templace(exp, "");
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
void sClean::redirStream(fstream &stm, string path, bool clear = false)
{
	if (stm.is_open()) // if there's a file already open
	{
		if (clear) { stm.clear(); }; // clear it if you're supposed to
		stm.close(); // then just close it
	}
	sClean::open(stm, path, false); // and then do the standard opening procedure
							 // loggy("the path has been directed to "+path);
}
void sClean::setRaw(string path)
{
	loggy("the raw stream path has been set to: " + path);
	redirStream(raw, path);
}
void sClean::setClean(string path)
{
	loggy("the output path has been set to: " + path);
	redirStream(cleaned, path, true);
}

string sClean::getFullPath(bool instream) // true means it's the input path
{										  // just to quickly concatenate that
	string fpath = instream ? fipath : fopath;
	fullPath = fpath + fname + ftype;
	// loggy("full path: "+fullPath);
	return fullPath;
}

string sClean::getTmpPath()
{
	// string fpath = instream ? fipath : fopath;
	fullPath = "tmp/" + tmpname + ftype;
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

// might turn this into its own class later tbh. otherwise, we need to store a var for, like, the default font size (which will probably be hard to determine this way... although maybe look at the spans when going through them and then seeing which one is closer to 1rem? and then storing larger and smaller spans for use of <big> and <small> later), default font name (in order to determine <pre> and <code> elements)
void sClean::Detector(vector<vector<string>> &els, string elm, string l)
{
	regex rule("(text-align|margin-left|text-decoration|background-color)"); // this covers both p and sp and yeah it's getting hard-coded for now
	if (regex_search(l, rule))
	{

		// cout << "this is a relevant " << elm << " rule! here's the full string we'll be looking at:\n" << l << "\n\n";
		vector<string> r;
		r.push_back(elm);
		const regex klass(elm + ".(\\S)+");
		smatch mitch;
		if (regex_search(l, mitch, klass))
		{
			r.push_back(mitch.str().substr(elm.length() + 1, mitch.str().length())); // should return just the class name
		}
		l = regex_replace(l, regex("(\\{|\\}|" + mitch.str() + "|\\s{2,})"), ""); // clean off the curly brackets, as well as the class substring and 2+ spaces
		string temp2;
		istringstream line;
		line.str(trim(l));

		while (getline(line, temp2, ';'))
		{
			if (regex_search(temp2, mitch, rule))
			{
				string mstr = trim(mitch.str());
				if (mstr == "margin-left")
				{
					if (regex_search(temp2, regex("\\d.\\d+")))
					{ // have to make sure this is, like. a proper blockquote el
						ruler(r, temp2);
						if (!regex_match(r[0], regex("h\\d")))
						{
							r[0] = "blockquote"; // so we can, like. i can't explain myself rn it'll be used in the sanitization process
						}
					}
				}
				else
				{
					ruler(r, temp2);
				}
			}
		}
		if (r.size() > 2)
		{					  // have to have at least 2 rules in order to be worth pushing back
			els.push_back(r); // and now finally. push the vector back into the master vector. though could probably also add in an additional clause to make sure it doesn't already match smth, that'll probably just overcomplicate things in the end
		}
	}
}
void sClean::ruler(vector<string> &v, string str)
{ // because it seems we cannot have local functions in c++ :pepehands:
	// so basically, we need to find the position of the ':' using regexp. we cut the rule off there.
	string t;
	istringstream s;
	s.str(str);
	do
	{ // have to do a do-while in order for it to pick up the whole thing for some reason. who knows why!
		if (!t.empty())
		{ // however, in doing so, we must also make sure that we don't push back the first empty string
			v.push_back(trim(t));
		}
	} while (getline(s, t, ':'));
}
void sClean::blockClean(string &tmp)
{
	bool cleanP{false};
	for (auto &p : impP)
	{
		if (regex_search(tmp, regex("class=\"" + p[1] + "\"")))
		{
			cleanP = true;
			strPt = &(p);
			break;
		}
	}

	// for now these get hard coded
	string emt{"p"};			 // default element is just a p
	string rpls{"<"}, rpls2{""}; // these are the value to be replaced. starts w/just the opening corner bracket as the string will have stuff appended to
	if (cleanP)
	{
		emt = (*strPt)[0]; // this is what renaming the elements in Detector() was for
		rpls += emt;
		for (int i{1}; 2 * i < (*strPt).size(); i++)
		{ // starts off w/2 bc values 0 & 1 are being used to hold the element + class names, then increase i by 2 bc the vector goes like key, value, key, value
				string key = (*strPt)[2 * i];
				string value = (*strPt)[2 * i + 1];
				if (key == "text-align")
				{
					rpls += " align=\"" + value + "\"";
				}
				// keep the inner <p> for blockquotes out of it for now i guess, since it's causing problems with the substitution. though i guess you could probably just clean that up in the nester f'n
				else if (key == "margin-left")
				{
					rpls += "><p"; // nest in that extra <p> for when we're working w/bqt
					rpls2 += "</p>";
				}
		}
		rpls += ">"; // then add in the closing bracket after the loop is done
	}
	else
	{
		// rpls = "<p>";
		rpls += emt + ">"; // just close the p otherwise lol
	}
	rpls2 += "</" + emt + ">";									   // make the closing element
	tmp = regex_replace(tmp, regex("</p>"), rpls2);				   // first remove the ending, in order to keep the blockquotes functioning properly
	tmp = regex_replace(tmp, regex("<p " + anyClass + ">"), rpls); // then you can replace the opening element
}

void sClean::spClean(string &tmp)
{
	bool cleanSp{false};
	vector<string> spInstances = {};
	vector<vector<string>> spRules = {};
	string currClass{""};
	string Orpls{""}, Erpls{""};
	// so first we determine if the current paragraph has any of the relevant classes
	for (auto &sp : impSp)
	{
		currClass = "class=\"" + sp[1] + "\"";
		if (regex_search(tmp, regex(currClass)))
		{
			cleanSp = true;
			spInstances.push_back(sp[1]);
			strPt = &(sp);
			// spClean(tmp, strPt);

			// this loop determines the html replacements
			for (int i{1}; 2 * i < (*strPt).size(); i++)
			{ // so the spans remain labelled spans, since they're more likely to have multiple nestings. or at least have that chance to be
				string key = (*strPt)[2 * i];
				string value = (*strPt)[2 * i + 1];
				if (key == "text-decoration")
				{
					Orpls += (value == "underline") ? "<ins>" : "<del>";			// if it's not an underline, then it's gonna be a strikethrough
					Erpls = ((value == "underline") ? "</ins>" : "</del>") + Erpls; // nest it inside
				}
				else if (key == "background-color")
				{
					Orpls += "<mark>";
					Erpls = "</mark>" + Erpls;
				}
			}
			spRules.push_back({Orpls, Erpls});
		}
	}

	if (cleanSp)
	{
		// regex spee("<span " + anyClass + ">((\\w|\\d|\\s)|^</span>)+</span>");
		// regex spee("<span " + anyClass + ">" /* this cover all the <span class="sx"> instances */ + "((\\w|\\d|\\s)+|^</span>)</span>");
		regex spee("<span " + anyClass + ">" /* this cover all the <span class="sx"> instances */ + "[^<]+</span>");
		// regex spee("<span " + anyClass + ">" + ".+" + "</span>", regex::nosubs | regex::optimize);
		// regex spee("<span " + anyClass + ">((\\w|\\W)|^</span>)+</span>");
		auto spStart = sregex_iterator(tmp.begin(), tmp.end(), spee); // so we basically have to pick out all the span elements
		auto spEnd = sregex_iterator();
		loggy("Found " + to_string(distance(spStart, spEnd)) + " <span> els in :\n\t" + tmp);

		int k{0};
		for (sregex_iterator i = spStart; i != spEnd; i++)
		{
			Orpls = ""; Erpls = ""; // reset these w/every loop
			// currClass = anyClass;	// reset this to anything at the start of each loop
			// smatch m = *i;
			string spn = (*i).str();
			k++;
			loggy("string match ["+ to_string(k) + "]: " + spn);
			bool ma{false};
			int j{0};
			// find out if it's an instance to be replaced
			// for (int j{0}; j < spInstances.size(); j++)
			while (j < spInstances.size() && !ma)
			{ // do this version since we're working with two vectors of the same size
				currClass = "class=\"" + spInstances[j] + "\"";
				if (regex_search(spn, regex(currClass)))
				{
					Orpls = spRules[j][0];
					Erpls = spRules[j][1];
					ma = true;
				}
				j++;
			}
			string clean = regex_replace(spn, regex("<span " + anyClass + ">"), Orpls); // so first we clean up the submatch. bc we're only working w/one <span> at a time, we can just do anyClass
			clean = regex_replace(clean, regex("</span>"), Erpls);						 // and then we clean
			loggy("this should be the cleaned string: " + clean +"\n");
			tmp = regex_replace(tmp, regex(spn), clean);
		}
	}
	// delete all other <span> instances indiscriminately
	tmp = regex_replace(tmp, regex("<span " + anyClass + ">"), "");
	tmp = regex_replace(tmp, regex("</span>"), "");
}

void sClean::open(fstream &stream, string path, bool append=false)
{
	if (!stream.is_open())
	{
		append ? stream.open(path, ios_base::app) : stream.open(path);
		if (!stream.is_open())
		{
			stream.clear();
			stream.open(path, ios::out);
			stream.close();
			append ? stream.open(path, ios_base::app) : stream.open(path);
		}
	}
}

void sClean::loggy(string str)
{ // will try to make these friends later but for now. whatever i guess.

	sClean::open(logger, "log.txt", true);
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	if (str.length() > 25) { str = "\n" + str; };
	logger << put_time(localtime(&now), "[%F %T]: ") << str << endl;
}
void sClean::loggy(vector<string> &strs)
{ // will try to make these friends later but for now. whatever i guess.
	sClean::open(logger, "log.txt", true);
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	logger << put_time(localtime(&now), "[%F %T]:\n");
	for (auto str : strs)
	{
		// logger << str << endl;
		loggy(str);
	}
}

void sClean::loggy(vector<vector<string>> &strs)
{
	time_t now = chrono::system_clock::to_time_t(chrono::system_clock::now());
	logger << put_time(localtime(&now), "[%F %T]:\n");
	for (int i{0}; i < strs.size(); i++)
	{
		logger << "Layer " << i << " of the 2D vector (" << strs[i].size() << " items):\n";
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
	raw.clear();
	cleaned.close();
	cleaned.clear();
	impP = {};
	impSp = {};
	// set the pointers to null just in case
	strPt = nullptr;
	vpt = nullptr;
}

void sClean::executor()
{				 // so how this should probably work to keep everything inside one while loop is that the major functions like findStyle() and stuff, when they're done running, they should basically switch the findEl w/in them, rather than have it happen here in the executor
	if (!bulk) { inputName(); } // set the name, if it's not a bulk
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
	loggy("findEl now searching for the style.");

	while (getline(raw, temp))
	{
		if (raw.eof())
		{
			cout << "\nEnd of file.\nOutput may be found in " << getFullPath(false) << ".\n";
			// loggy("\nEnd of file.\nOutput may be found in " + getFullPath(false) + ".\n");
			// foundEl = false; //set this to false for subsequent runs it seems
			// reset();
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
				// loggy(el + " " + to_string(numLines) +": " + temp);
				findStyle(); // if the style switch is off, then do findStyle();
			}
			else if (!bodySw)
			{
				sanitize(); // otherwise, we're sanitizing the body
							// if (numLines%25 == 0) { //log every 10 lines bc i'm not trying to be that much of a menace
							// 	loggy(el + " " + to_string(numLines) +": " + temp);
							// }
			}
		}
		if (regex_search(temp, elStart))
		{
			foundEl = true;
			cout << "Start of " << el << " found on line " << numLines << ".\n";
			// loggy("Start of " + el + " found on line " + to_string(numLines) + ".\n");
		}
		numLines++;
	}

	// reset these outside of the while loop in order to keep it from looping twice.
	setRaw(getTmpPath());
	setClean(getFullPath(false));
	nester(); // then finally, take the temp file and output it somewhere proper

	reset();
	// loggy("End of logging for: " + fname + "\n");
}