#ifndef SCRIV2AO3
#define SCRIV2AO3

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
using namespace std;

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
void snip(string &s, int i)
{
	try
	{
		s.erase(s.begin(), s.begin() + i);
	}
	catch (out_of_range)
	{
		cout << "out of range!!" << endl;
		s = "";
	}
}

class cssRule
{
public:
	cssRule() {};	 // default constructor
	cssRule(string); // constructor from css stylesheet rule
	cssRule(string e, string c, string g);

	string printTag();
	string printClose();
	string printParent();
	string closeParent();
	void setIndex(int);

	// fuck it. everyone's public
	string el{""}, klass{""}, rulez{""}, display{"inline"}, guts{""}, parent{""}; // "parent" is used for things like blockquotes n lists, which have important nesting happening

	bool bqtMode{false}, listMode{false}, fsSpecified{false}, worthwhile{false}, parentage{false}; // booleans for when we're working w/blockquotes n stuff

	static vector<cssRule> stylesheet; // static vector stylesheet

	int tableIndex{0}; // in case we have adjacent tables, this keeps track of which one we're on

	float fontSize{0.0}; // measured in rems

private:
	// int indent{0};
protected:
	string innerHTML{""};
};

class sanitize : public cssRule
{
public:
	sanitize() {};
	sanitize(string);
	sanitize(string, cssRule);
	sanitize(stringstream &, cssRule);
	sanitize(istringstream &, cssRule);

	void init(string s);

	string cleanup();
	string findAndSanitize(string &str);
	string str();
	void debug(bool inclHTML);
	// operator overloading
	friend ostream &operator<<(ostream &, const sanitize &); // printing out the thing out
	sanitize operator+=(const string &);
	sanitize operator+=(const stringstream &);
	sanitize operator+=(const istringstream &);
	sanitize operator+=(const sanitize &);
	sanitize operator+=(const int &i);
	sanitize operator-=(const int &i);

	sanitize operator++();
	sanitize operator++(int);
	sanitize operator--();
	sanitize operator--(int);
	long unsigned int length();
	long unsigned int size();
	long unsigned int rawSize();
	void reset();

	static bool prettify;
	static char fill;
	static string hrStr;

private:
protected:
	cssRule rule;
	bool hr{false}; // horizontal rule type tags
	long unsigned int indent{0};	// rowCells for knowing the number of children a <tr> has

	vector<string> unnestings{"em", "strong"}; // elements needing to be unnested

	// this is kind of hacked together for the moment, but it works for how the sanitizer currently functions
	vector<int> indeces{0}; // starting index is always 0
};

/* sanitize operator overloads */ 
/* they get sad if they're moved to another file */
ostream &operator<<(ostream &os, const sanitize &san)
{
	sanitize copy(san);
	string nya{copy.cleanup()}; // might have to make this a stringstream
	os << setfill(sanitize::fill);
	if (!copy.hr)
	{
		os << copy.printTag();
	}
	if (copy.indeces.size() > 1 && sanitize::prettify)
	{
		if (copy.hr)
		{
			cout << "ohh. hmm. i suspect we were not supposed to splitter this. yet here we are, doing it anyway." << endl;
		}

		int i{0}; // keeps track of where in the string we currently are
		for (int j{0}; j < copy.indeces.size(); j++)
		{
			if (copy.indeces[j] > 0) // prevent it from printing useless tabs n stuff
			{
				os << endl;						   // start a new line
				os << setw(copy.indent + 1) << ""; // tab it
				os << nya.substr(i, copy.indeces[j]);
			}
			i += copy.indeces[j]; // add this for the next loop's starting point
		}
		os << endl
		   << setw(copy.indent) << ""; // and then tab it in preparation for the closing tag
	}
	else
	{
		// otherwise, just print it as it is
		os << nya;
	}

	if (!copy.hr)
	{
		os << copy.printClose();
	}

	return os;
}


// initialize the statics
bool sanitize::prettify = true;
char sanitize::fill = '\t';
string sanitize::hrStr = "~***~"; // must either be an exact string or a regex
vector<cssRule> cssRule::stylesheet = {};

class li
{
	// actually i think it might be time to make a class for this one.
public:
	li() {};
	li(string);
	string cleanSpan(string);
	bool incl(string);
	friend ostream &operator<<(ostream &, const li &);

private:
	string sp{"(<span\\sclass=\"((\\w|\\d|\\-)+)\">)(.*?)(</span>)"}; // regex for spans i fucking guess
protected:
	string clean{""}, tmp{""};
	cssRule r;
};

ostream &operator<<(ostream &os, const li &l)
{
	os << l.clean;
	return os;
}

class ruby
{
	// should have two cssRules, one for the base n one for the text
public:
	ruby() {};
	ruby(string r)
	{
		//
	}

	friend sanitize &operator<<(const sanitize &, const ruby &);

private:
protected:
	string rubyBase{""}, rubyText{""};
};

#endif