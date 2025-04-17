#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "options.h"
#include "whatever.h"

using namespace std;

cssRule::cssRule(string r)
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
		else if (i == 3) // the class
		{
			klass = ((tmp2 != "") ? tmp2.substr(1) : ""); // cut off the period if it's not blank
		}
		else if (i == 5)
		{
			rulez = tmp2.substr(1, (tmp2.length() - 2)); // cut off the {} from the rules
		}
		i++;
	}
	// if (el != "span")
	if (!(el == "span" || el == "em" || el == "ins" || el == "del" || el == "strong"))
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
	istringstream tmp(rulez);
	while (getline(tmp, tmp2, ';'))
	{
		tmp2 = trim(tmp2);

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
cssRule::cssRule(string p, string c)
{
	// so far only really used for ruby text so eh?
	el = c;
	parent = p;
}
cssRule::cssRule(string e, string c, string g)
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
		el = "li";
		listMode = true;
	}
	else if (el == "table")
	{
		el = "tr";
		display = "table";
	}
	else if (el == "ruby" || el == "rb" || el == "rbc" || el == "rt" || el == "rtc")
	{
		display = "ruby";
		// and then later we'll have to do some more specific stuff for parentage n whatever
	}

	parentage = (el != parent); // idk. might be useful later i suppose
}

string cssRule::printTag() { return "<" + el + guts + ">"; } // making the printer check means this shit will proceed to take five million billion years to do so.
string cssRule::printClose() { return "</" + el + ">"; }
string cssRule::printParent() { return "<" + parent + ">"; }
string cssRule::closeParent() { return "</" + parent + ">"; }

void cssRule::setIndex(int i) { tableIndex = i; } // ...we have to do it this way bc the ++ and -- operators are for the indents. ehe

/* sanitize */
/* constructors  */
sanitize::sanitize(string s) : cssRule::cssRule() { init(s); }
sanitize::sanitize(string s, cssRule r) : cssRule::cssRule(r) { init(s); }
sanitize::sanitize(stringstream &s, cssRule r) : cssRule::cssRule(r) { init(s.str()); }
sanitize::sanitize(istringstream &s, cssRule r) : cssRule::cssRule(r) { init(s.str()); }

void sanitize::init(string s)
{

	if (parent != el)
	{
		indent = 1;
	}
	innerHTML = s;
	if (s == sanitize::hrStr)
	{
		hr = true;
	}
}

string sanitize::cleanup()
{
	string tmp = innerHTML; // leave the actual inner html untouched
	if (hr || tmp == sanitize::hrStr || regex_search(tmp, regex(string("^(<.*?>)*?" + sanitize::hrStr + "(<.*?>)*?$"))))
	{
		hr = true;
		tmp = "<hr />";
	}
	else if ((tmp == "") || regex_search(tmp, regex("^(<(em|strong)>)*(\\s+|<br\\s?/?>)(</(em|strong)>)*$")))
	{
		// so if it's an empty string or just spaces
		tmp = "&nbsp;"; // replace it with this nbsp
	}
	else
	{
		if (rawSize() > 8192)
		{
			string cleanTmp{""};
			long unsigned int i{0}, spacer{2048};
			while (i < rawSize())
			{
				// let's do this in nice safe increments of 2048
				// string sub{tmp.substr(i, i + 2048)};
				// sub = ;
				string sub{tmp.substr(i, i + spacer)};
				// then do some checks to make sure we're not splitting up any escape codes
				cleanTmp += findAndSanitize(sub);
				i += sub.length();
			}
			tmp = cleanTmp; // set it to this
		}
		else
		{
			tmp = findAndSanitize(tmp);
		}
	}

	return tmp; // for now just empty spaces
}

string sanitize::findAndSanitize(string &str)
{
	string tmp{str};
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
		while (regex_search(tmp, aiya))
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

	// regex ruby("\\((.+?)\\s\\|\\s(.+?)\\)"); // (<[^>]*?>)?
	// regex rby(ruby::rubyregex); // can we do this. i kind of don't think so.
	// and then perhaps finally, ruby text
	smatch s;
	if (regex_search(tmp, s, regex(ruby::rubyregex)))
	{
		cout << "the search result match in s: " << s.str() << endl;
		// new rubinator i guess
		// rubinator(tmp);
		ruby roo(s.str()); // make a ruby out of it
	}
	return tmp;
}

string sanitize::str()
{
	stringstream wa("");
	wa << *this;
	return wa.str(); // stringify the clean html lol
}

void sanitize::debug(bool inclHTML)
{
	// print out all the vars
	cout << "\nDEBUGGING..." << endl
		 << endl;
	cout << "INHERITED FROM cssRule:" << endl;
	cout << "STRINGS:" << endl;
	cout << "el: " << el << "\tklass: " << klass << "\tdisplay: " << display << endl;
	cout << "parent: " << parent << "\tguts: " << guts << endl;
	cout << "BOOLS:" << endl;
	cout << "bqtMode: " << bqtMode << "\tlistMode: " << listMode << "\tfsSpecified: " << fsSpecified << "\tworthwhile: " << worthwhile << "\tparentage: " << parentage << endl;
	cout << "NUMBERS: " << endl;
	cout << "length: " << length() << "\tsize: " << size() << "\traw size: " << rawSize() << endl
		 << endl;

	if (inclHTML)
	{
		cout << "innerHTML:\n\t" << innerHTML << endl;
	}
	cout << "\n----------------\n\n\n";
}

long unsigned int sanitize::length() { return indent; }
long unsigned int sanitize::size() // about to do some Insane Shit i'm sure but anyway where length() gets the indent, size() gets the string length
{
	stringstream nya("");
	nya << *this;
	return nya.str().length();
}
long unsigned int sanitize::rawSize() { return innerHTML.length(); }

void sanitize::reset()
{
	innerHTML = "";
	rule = cssRule();
	indent = !(parent == el);
	tableIndex = 0;
	indeces = {0}; // reset this
}

/* sanitize operator overloads */
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

sanitize sanitize::operator+=(const string &str)
{
	innerHTML += str; // concatenate & continue
	return *this;
};
sanitize sanitize::operator+=(const stringstream &str)
{
	innerHTML += str.str();
	return *this;
};
sanitize sanitize::operator+=(const istringstream &str)
{
	innerHTML += str.str();
	return *this;
};
sanitize sanitize::operator+=(const sanitize &san) // when adding two sanitizes together, you're appending one as an el to the other
{
	sanitize copy(san);
	innerHTML += copy.printTag() + san.innerHTML + copy.printClose(); // maybe should also have this be a cleaned up version? so that if we continue this nesting nonsense, those children also end up properly tabbed as well

	indeces.push_back(copy.size()); // and then also add the index of the child... later probably also make it acct for when mixing strings and elements
	return *this;
}
sanitize sanitize::operator+=(const int &i) // ya we can += integers too
{
	indent += i;
	return *this;
}
sanitize sanitize::operator-=(const int &i)
{
	indent -= i;
	return *this;
}
sanitize sanitize::operator++()
{
	indent++;
	return *this;
}
sanitize sanitize::operator++(int)
{
	sanitize o = *this;
	operator++();
	return o;
}
sanitize sanitize::operator--()
{
	indent--;
	return *this;
}
sanitize sanitize::operator--(int)
{
	sanitize o = *this;
	operator--();
	return o;
}

//
