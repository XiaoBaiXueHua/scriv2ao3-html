#ifndef CSSRULEZ
#define CSSRULEZ

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

	cssRule(string, string); // constructor for just parent and child

	cssRule(string, string, string); // constructor from el, class, n guts

	string printTag();
	string printClose();
	string printParent();
	string closeParent();
	void setIndex(int);
	void setHTML(string);
	string getHTML();

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
	vector<int> indeces{0}; // starting index is always 0
};
#endif

#ifndef RUBINATOR
#define RUBINATOR
class ruby // ...it turns out ao3 doesn't allow for <rb> and <rbc> elements, so there's no point in trying to do elaborate ruby splitting for now
{
	// should have two cssRules, one for the base n one for the text
public:
	// ruby() {};
	// ruby(string);

	static string rubyregex;
	// static int splitOpt; // this gets enumerated in the config function process
	static bool process;
	static pair<int, int> weh; // this is for the submatch indeces for the ruby base n ruby text

	friend class sanitize;
	// friend sanitize &operator<<(sanitize &, const ruby &);

private:
// smatch smodge;
protected:
	// string rubyBase{""}, rubyText{""}, orig{""};
	// cssRule rParent{"ruby", "ruby"}, rb{"rbc", "rb"}, rt{"rtc", "rt"};
};

// int ruby::splitOpt{5}; // default is nosplit, since apparently ao3 doesn't actually allow for <rb> and <rbc> tags.
bool ruby::process{true};
string ruby::rubyregex{"\\((.*?)\\s\\|\\s(.*?)\\)"};
pair<int, int> ruby::weh = make_pair(1, 2);
#endif

#ifndef SANITIZER
#define SANITIZER
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
	string findAndSanitize(string &);
	string str();
	void debug(bool);

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

	friend class ruby;

private:
protected:
	cssRule rule;
	bool hr{false};				 // horizontal rule type tags
	long unsigned int indent{0}; // rowCells for knowing the number of children a <tr> has

	vector<string> unnestings{"em", "strong"}; // elements needing to be unnested

	// this is kind of hacked together for the moment, but it works for how the sanitizer currently functions
};

// initialize the statics
bool sanitize::prettify = true;
char sanitize::fill = '\t';
string sanitize::hrStr = "~***~"; // must either be an exact string or a regex
vector<cssRule> cssRule::stylesheet = {};

#endif