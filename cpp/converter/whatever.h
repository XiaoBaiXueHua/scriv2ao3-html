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
			tmp2 = s.str(); // hold the string
			if (i == 2)
			{
				el = tmp2;
				parent = el;
			}
			else if (i == 3)
			{												  // the class
				klass = ((tmp2 != "") ? tmp2.substr(1) : ""); // cut off the period if it's not blank
			}
			else if (i == 5)
			{
				rulez = tmp2.substr(1, (tmp2.length() - 2)); // cut off the {} from the rules
			}
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
			{
				tmp3 = trim(tmp3);
				tmpVec.push_back(tmp3); // push back a trimmed version
			}
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
						parent = "blockquote";
						// el = "blockquote";
						// guts = "><p" + guts;
						bqtMode = true;
						// indent++;
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
		parent = el;
		if (el == "blockquote" || el == "ol" || el == "ul")
		{
			// parent = el;
			// indent++;
		}

		if (el == "blockquote")
		{
			el = "p";
			bqtMode = true;
		}
		else if (el == "ol" || el == "ul")
		{
			// parent = el;
			el = "li";
			listMode = true;
		}
	}


	string printTag() { return "<" + el + guts + ">"; }
	string printClose() { return "</" + el + ">"; }
	string printParent() { return "<" + parent + ">"; }
	string closeParent() { return "</" + parent + ">"; }

	// the only thing we should be increasing/decreasing is the indent so like y'know
	// cssRule &operator++()
	// {
	// 	indent++;
	// 	return *this;
	// }
	// cssRule &operator--()
	// {
	// 	indent--;
	// 	return *this;
	// }

	// fuck it. everyone's public

	string el{""}, klass{""}, rulez{""}, display{"inline"}, guts{""}, parent{""}; // "parent" is used for things like blockquotes n lists, which have important nesting happening

	bool bqtMode{false}, listMode{false}, fsSpecified{false}, worthwhile{false}; // booleans for when we're working w/blockquotes n stuff

	float fontSize{0.0}; // measured in rems

private:
	// int indent{0};
};

class sanitize : public cssRule
{
public:
	sanitize() {};
	sanitize(string s, cssRule r) : cssRule(r)
	{
		init(s);
	}
	sanitize(stringstream &s, cssRule r) : cssRule(r)
	{
		init(s.str());
	}
	sanitize(istringstream &s, cssRule r) : cssRule(r)
	{
		init(s.str());
	}

	void init(string s)
	{
		if (parent == "blockquote") {
			indent = 1;
		}
		if (s == "~***~")
		{
			hr = true;
		}
		else
		{
			innerHTML = s;
		}
	}

	string cleanup()
	{
		string tmp = innerHTML; // leave the actual inner html untouched
		if (hr)
		{
			tmp = "<hr />";
		}
		else if ((tmp == "") || regex_search(tmp, regex("^(<(em|strong)>)*(\\s+|<br\\s?/>)(</(em|strong)>)*$")))
		{
			// so if it's an empty string or just spaces
			tmp = "&nbsp;"; // replace it with this nbsp
		}
		else if (regex_search(tmp, regex("&#\\d+;")))
		{
			// loop through and cleanup any sort of &#numbers; html code things

			// cout << "now to replace the fucking &#\\d+; things in the line, \"" << tmp << "\"" << endl;
			// string subTmp = tmp; // hold this
			// tmp = ""; // clear this out
			regex aiya{"(&#(\\d+);)(.)?"}; // this reults in 0 = full thing, 1 = just the numbers, and then 2 = whatever comes after
			while (regex_search(tmp, regex("&#\\d+;")))
			{
				sregex_iterator oi{tmp.begin(), tmp.end(), aiya};
				regex rpls(string((*oi)[1].str())); // should be the full thing
				int num{stoi(string((*oi)[2].str()))};
				string m{char(num)};
				if (num == 60 && string((*oi)[1].str()) == "3")
				{
					// if it's a gt sign & followed by a 3
					m = "&gt;"; // make it a gt instead
				}
				tmp = regex_replace(tmp, rpls, m);
			}
		}
		return tmp; // for now just empty spaces
	}

	friend ostream &operator<<(ostream &, const sanitize &); // printing out the thing out
	sanitize operator+=(const string &str)
	{
		innerHTML += str; // concatenate & continue
		return *this;
	};
	sanitize operator+=(const stringstream &str)
	{
		innerHTML += str.str();
		return *this;
	};
	sanitize operator+=(const istringstream &str)
	{
		innerHTML += str.str();
		return *this;
	};
	sanitize operator+=(const sanitize &san)
	{
		innerHTML += san.innerHTML;
		return *this;
	}
	sanitize operator++()
	{
		indent++;
		// indent++;
		return *this;
	}
	sanitize operator++(int) {
		sanitize o = *this;
		operator++();
		return o;
	}
	sanitize operator--()
	{
		indent--;
		return *this;
	}
	sanitize operator--(int) {
		sanitize o = *this;
		operator--();
		return o;
	}
	int length() { return indent + 1; }

	void reset()
	{
		innerHTML = "";
		rule = cssRule();
	}

private:
	string innerHTML{""};
	cssRule rule;
	bool hr{false}; // horizontal rule type tags
	int indent{0};
};

ostream &operator<<(ostream &os, const sanitize &san)
{

	string nya{sanitize(san).cleanup()}; // sanitized version of the otherwise raw innerHTML
	if (!san.hr)
	{
		nya = sanitize(san).printTag() + nya + sanitize(san).printClose();
	}

	os << nya;

	return os;
}