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
			if (el == "table" || el == "th" || el == "tbody" || el == "thead" || el == "td" || el == "tr")
			{
				// and now if we're working w/table elements
				display = "table";
			}
			else
			{
				display = "block";
			}
		}
		// cout << "el: " << el << "\t\tklass: " << klass << "\trulez: " << rulez << endl
		//  << endl;
		istringstream tmp(rulez);
		while (getline(tmp, tmp2, ';'))
		{
			// tmp2 = string(regex_replace(regex_replace(tmp2, regex("^\\s*"), ""), regex("\\s*$"), "")); // trim
			// tmp2 = cssRule::trim(tmp2);
			tmp2 = trim(tmp2);
			// cout << tmp2 << endl;

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
						bqtMode = true;
					}
				}
			}
		}
		
		if ((el == "blockquote") || (el == "ul") || (el == "ol"))
		{
			parentage = true;
		}

		// turning the list mode on depending on the element
		if ((el == "ul") || (el == "ol"))
		{
			listMode = true;
		}
	}
	cssRule(string e, string c, string g)
	{
		// can also just make one from an element, class, and guts
		el = e;
		klass = c;
		guts = g;
		parent = el;

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
		else if (el == "table")
		{
			el = "tr";
			display = "table";
		}

		parentage = (el != parent); // idk. might be useful later i suppose
	}

	string printTag() { return "<" + el + guts + ">"; }
	string printClose() { return "</" + el + ">"; }
	string printParent() { return "<" + parent + ">"; }
	string closeParent() { return "</" + parent + ">"; }
	void setIndex(int i)
	{
		tableIndex = i; // ...we have to do it this way bc the ++ and -- operators are for the indents. ehe
	}

	// fuck it. everyone's public
	string el{""}, klass{""}, rulez{""}, display{"inline"}, guts{""}, parent{""}; // "parent" is used for things like blockquotes n lists, which have important nesting happening

	bool bqtMode{false}, listMode{false}, fsSpecified{false}, worthwhile{false}, parentage{false}; // booleans for when we're working w/blockquotes n stuff

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
	sanitize(string s) : cssRule() { init(s); }
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
		if (parent != el)
		{
			indent = 1;
		}
		innerHTML = s;
	}

	string cleanup()
	{
		string tmp = innerHTML; // leave the actual inner html untouched
		if (hr || tmp == "~***~")
		{
			hr = true;
			tmp = "<hr />";
		}
		else if ((tmp == "") || regex_search(tmp, regex("^(<(em|strong)>)*(\\s+|<br\\s?/>)(</(em|strong)>)*$")))
		{
			// so if it's an empty string or just spaces
			tmp = "&nbsp;"; // replace it with this nbsp
		}
		else
		{
			// fix nesting issues
			// first swap the order of consecutive <strong><em>s

			regex misnested("<strong><em>"); // just hard code it for now i suppose
			if (regex_search(tmp, misnested))
			{
				while (regex_search(tmp, misnested))
				{
					tmp = regex_replace(tmp, misnested, "<em><strong>");
				}

				// now the closing tags
				misnested = "</em></strong>";
				while (regex_search(tmp, misnested))
				{
					tmp = regex_replace(tmp, misnested, "</strong></em>");
				}
			}

			// make the regex
			string ack{""};
			for (int i{0}; i < unnestings.size(); i++)
			{
				string e{unnestings[i]};
				ack += "</" + e + "><" + e + ">";
				if (i < unnestings.size() - 1)
				{
					ack += "|"; // add the pipe as well
				}
			}
			ack = "(" + ack + ")"; // stick it all into a parenthetical

			// then actually search and replace
			regex guh(ack);
			if (regex_search(tmp, guh))
			{
				while (regex_search(tmp, guh))
				{
					tmp = regex_replace(tmp, guh, "");
				}
			}

			if (regex_search(tmp, regex("&#\\d+;")))
			{
				// loop through and cleanup any sort of &#numbers; html code things

				regex aiya{"(&#(\\d+);)(.)?"}; // this reults in 0 = full thing, 1 = just the numbers, and then 2 = whatever comes after
				while (regex_search(tmp, regex("&#\\d+;")))
				{
					sregex_iterator oi{tmp.begin(), tmp.end(), aiya};
					sregex_iterator end;
					while (oi != end)
					{
						regex rpls(string((*oi)[1].str())); // should be the full thing
						int num{stoi(string((*oi)[2].str()))};
						string m{char(num)};
						if (num == 60 && string((*oi)[1].str()) == "3")
						{
							// if it's a gt sign & followed by a 3
							m = "&gt;"; // make it a gt instead
						}
						tmp = regex_replace(tmp, rpls, m);
						oi++;
					}
				}
			}

			regex ruby("\\((.+?)\\s\\|\\s(.+?)\\)"); // (<[^>]*?>)?
			// and then perhaps finally, ruby text
			if (regex_search(tmp, ruby))
			{
				// new rubinator i guess
				// rubinator(tmp);
			}

			// then, once all that's been done, we also print out the children
		}

		return tmp; // for now just empty spaces
	}
	sanitize rubinator(string s)
	{
		sanitize r(s, cssRule("ruby", "", ""));	 //
		regex ruby("\\((.+?)\\s\\|\\s(.+?)\\)"); // (<[^>]*?>)?
		string tmp{s}, tmp2{""};
		// innerHTML = s;
		cout << "Ruby text swapout time" << endl;
		// sregex_iterator r(tmp.begin(), tmp.end(), ruby);
		// for (auto const &b : *r)
		// {
		// 	cout << b << endl;
		// }
		string rbc{""}, rtc{""};		   // strings for the eventual rb and rt elements, + another tmp
		vector<string> rbv = {}, rtv = {}; // vectors for the things as we go through it

		// the rough and tumble way to do this would be to just split them up by word and completely ignore

		// stringstream o((*r)[1]); // make a stringstream from the first half (the ruby base)
		// while(getline(o, tmp2, ' ')) {
		// 	// go by word
		// 	//
		// }
		return r;
	}

	string str()
	{
		stringstream wa("");
		wa << *this;
		return wa.str(); // stringify the clean html lol
	}

	// operator overloading
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
	sanitize operator+=(const sanitize &san) // when adding two sanitizes together, you're nesting one inside the other
	{
		sanitize copy(san);
		innerHTML += copy.printTag() + san.innerHTML + copy.printClose(); // maybe should also have this be a cleaned up version? so that if we continue this nesting nonsense, those children also end up properly tabbed as well
		indeces.push_back(copy.size());									  // and then also add the index of the child... later probably also make it acct for when mixing strings and elements
		return *this;
	}
	sanitize operator+=(const int &i) // ya we can += integers too
	{
		indent += i;
		return *this;
	}
	sanitize operator-=(const int &i)
	{
		indent -= i;
		return *this;
	}

	sanitize operator++()
	{
		indent++;
		return *this;
	}
	sanitize operator++(int)
	{
		sanitize o = *this;
		operator++();
		return o;
	}
	sanitize operator--()
	{
		indent--;
		return *this;
	}
	sanitize operator--(int)
	{
		sanitize o = *this;
		operator--();
		return o;
	}
	int length() { return indent; }
	int size() // about to do some Insane Shit i'm sure but anyway where length() gets the indent, size() gets the string length
	{
		stringstream nya("");
		nya << *this;
		return nya.str().length();
	}
	// void incCells() { rowCells++; }
	// void clearCells() {
	// 	rowCells = 0;
	// }
	void reset()
	{
		innerHTML = "";
		rule = cssRule();
		indent = !(parent == el);
		tableIndex = 0;
		indeces = {0}; // reset this
	}

private:
protected:
	// string innerHTML{""};
	cssRule rule;
	bool hr{false}; // horizontal rule type tags
	int indent{0};	// rowCells for knowing the number of children a <tr> has

	vector<string> unnestings{"em", "strong"}; // elements needing to be unnested

	// this is kind of hacked together for the moment, but it works for how the sanitizer currently functions
	vector<int> indeces{0}; // starting index is always 0
};

ostream &operator<<(ostream &os, const sanitize &san)
{
	sanitize copy(san);
	string nya{copy.cleanup()};
	os << setfill('\t');
	if (!copy.hr)
	{
		os << copy.printTag();
	}
	if (copy.indeces.size() > 1)
	{
		if (copy.hr) {
			cout << "ohh. hmm. i suspect we were not supposed to splitter this. yet here we are, doing it anyway." << endl;
		}
		// cout << "this " << copy.printTag() << " has " << copy.indeces.size() << " children." << endl;
		// if there are child elements, then slice them up
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
	// string nya{copy.cleanup()}; // sanitized version of the otherwise raw innerHTML
	// if (!copy.hr)
	// {
	// 	nya = copy.printTag() + nya + copy.printClose();
	// }

	// os << nya;

	return os;
}
// sanitize &operator<<(sanitize &san, const ostream &os) {
// 	// uhhhh y'know i really did not think i'd get this far.
// 	sanitize copy(san);
// 	copy << os;
// 	return copy;
// }