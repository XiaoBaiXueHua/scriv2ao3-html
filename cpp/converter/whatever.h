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

class cssRule
{
public:
	cssRule() {}; // default constructor
	cssRule(string r)
	{
		// feed it a line, which should already be trimmed, but y'know
		regex cssLine("((\\w+)(\\.(\\w|\\d|-)+)?\\s(\\{.*\\}))");
		// cout << tmp << endl;
		// regex cssLine(".*"); // just fucking anything lol
		string tmp2{""};
		sregex_iterator yee(r.begin(), r.end(), cssLine);

		int i{0};
		for (auto s : *(yee))
		{
			// cout << "\tsubmatch " << i << ": " << s << endl;
			tmp2 = s.str(); // hold the string

			// if ((i == 2) || (i == 3) || (i == 5)) {
			if (i == 2)
			{
				el = tmp2;
			}
			else if (i == 3)
			{												  // the class
				klass = ((tmp2 != "") ? tmp2.substr(1) : ""); // cut off the period if it's not blank
			}
			else if (i == 5)
			{
				rulez = tmp2.substr(1, (tmp2.length() - 2)); // cut off the {} from the rules
			}
			// cssRules.push_back(tmp2);
			// }
			i++;
		}
		if (el != "span")
		{
			display = "block";
		}
		cout << "el: " << el << "\t\tklass: " << klass << "\trulez: " << rulez << endl
			 << endl;
		istringstream tmp(rulez);
		while (getline(tmp, tmp2, ';'))
		{
			// tmp2 = string(regex_replace(regex_replace(tmp2, regex("^\\s*"), ""), regex("\\s*$"), "")); // trim
			// tmp2 = cssRule::trim(tmp2);
			tmp2 = trim(tmp2);
			cout << tmp2 << endl;

			istringstream wah(tmp2);
			string tmp3{""};
			// tmpVec.clear();
			vector<string> tmpVec{};
			while (getline(wah, tmp3, ':'))
			{ //
				// tmp3 = cssRule::trim(tmp3);
				tmp3 = trim(tmp3);
				tmpVec.push_back(tmp3); // push back a trimmed version
			}
			// cout << "tmpVec stuff: " << endl;
			// cout << "\t" << tmpVec[0] << "\t" << tmpVec[1] << endl;
			// leave font-weight n font-style alone since scrivener already provides
			tmp3 = tmpVec[0];
			if ((tmp3 == "font-size" || tmp3 == "text-decoration" || tmp3 == "text-align" || tmp3 == "margin-left") && el != "body")
			{
				worthwhile = true; // these rules are worth keeping
				float misc{0.0};   // float to help out
				if (tmpVec[0] == "font-size")
				{
					cout << "setting the font size." << endl;
					// tmp3 = regex_replace(tmpVec[1], regex("\\w+$"), "");
					// tmp3 = trim();
					fontSize = stof(tmpVec[1]);
					fsSpecified = true;
				}
				else if (tmpVec[0] == "text-decoration")
				{
					el = (tmpVec[1] == "underline") ? "ins" : "del";
				}
				else if (tmpVec[0] == "text-align")
				{
					guts += " align=\"" + tmpVec[1] + "\"";
				}
				else if (tmpVec[0] == "margin-left")
				{
					// blockquotes
					misc = stof(tmpVec[1]);
					if (misc >= 1)
					{
						el = "blockquote";
						guts = "><p" + guts;
						bqtMode = true;
					}
				}
			}

			// turning the list mode on depending on the element
			if ((el == "ul") || (el == "ol"))
			{
				listMode = true;
				worthwhile = true;
			}
		}
	}
	cssRule(string e, string c, string g)
	{
		// can also just make one from an element, class, and guts
		el = e;
		klass = c;
		guts = g;
		if (el == "blockquote") {
			bqtMode = true;
		} else if (el == "ol" || el == "ul") {
			listMode = true;
		}
	}

	static string trim(string str)
	{
		return string(regex_replace(regex_replace(str, regex("^\\s*"), ""), regex("\\s*$"), ""));
	}

	string printTag() { return "<" + el + guts + ">"; }
	string printClose() { return ((string(bqtMode ? "</p>" : "")) + "</" + el + ">"); }

	// fuck it. everyone's public
	string el{""}, klass{""}, rulez{""}, display{"inline"}, guts{""};
	bool bqtMode{false}, listMode{false}, fsSpecified{false}, worthwhile{false}; // booleans for when we're working w/blockquotes n stuff

	float fontSize{0.0}; // measured in rems

private:
};

class sanitize
{
public:
	sanitize() {};
	sanitize(string s, cssRule r)
	{
		init(s, r);
	}
	sanitize(stringstream &s, cssRule r)
	{
		init(s.str(), r);
	}
	sanitize(istringstream &s, cssRule r)
	{
		init(s.str(), r);
	}

	void init(string s, cssRule r) {
		rule = r;
		// elaborate on this later, but for now just hr elements
		if (s == "~***~") {
			hr = true; 
		} else {
			innerHTML = s;
		}
	}
	string cleanup() {
		return (((innerHTML != "") || regex_search(innerHTML, regex("^\\s+$"))) ? innerHTML : "&nbsp;"); // for now just empty spaces
	}

	friend ostream &operator<<(ostream &, const sanitize &); // printing out the thing out
	sanitize operator+=(const string &str) {
		innerHTML += str; // concatenate & continue
		return *this;
	};
	sanitize operator+=(const stringstream &str) {
		innerHTML += str.str();
		return *this;
	};
	sanitize operator+=(const istringstream &str) {
		innerHTML += str.str();
		return *this;
	};
	sanitize operator+=(const sanitize &san) {
		innerHTML += san.innerHTML;
		return *this;
	}
	void reset() {
		innerHTML = "";
		rule = cssRule();
	}

private:
	string innerHTML{""};
	cssRule rule;
	bool hr{false}; // horizontal rule type tags
};

ostream &operator<<(ostream &os, const sanitize &san)
{
	
	cssRule r(san.rule);
	string nya = sanitize(san).cleanup(); // sanitized version of the otherwise raw innerHTML
	// cout << "printing: \n\t" << nya << endl;
	nya = (san.hr) ? ("<" + r.el + r.guts + " />") : string(r.printTag() + nya + r.printClose());

	os << nya;
	
	return os;
}
// sanitize operator+=(const string &str) {
// 	// san.innerHTML = san.innerHTML + str;
// 	// san.innerHTML = san.innerHTML;
// 	// return san.innerHTML + str; // just concatinate them
// }
// sanitize operator+=(const istringstream &str, const sanitize &san) {
// 	// return san.innerHTML + str.str(); // just concatinate them
// }