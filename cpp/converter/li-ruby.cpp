
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "whatever.h" // has dependencies on cssRule and sanitize
#include "li-ruby.h"

using namespace std;


/* li */
li::li(string s)
{
	tmp = s;
	regex spanner(sp);
	if (regex_search(tmp, spanner))
	{
		long long unsigned int i;
		while (regex_search(tmp, spanner))
		{

			i = {tmp.find("<span")};
			if (i > 0) //
			{
				clean += tmp.substr(0, i); // add the orphans up front of the span
				snip(tmp, i);
			}

			i = tmp.find("</span>") + 7; // automatically account for the length of the </span>
			string spn{tmp.substr(0, i)};
			clean += cleanSpan(spn);
			snip(tmp, i);
		}
		// then add the rest of tmp to the clean line
		clean += tmp;
	}
	else
	{
		clean = tmp; // otherwise we're free! we don't need to do any cleaning
	}
}

string li::cleanSpan(string span)
{
	string t{""};
	regex s(sp);
	// match_results sm = regex_match(span, s);
	smatch sm; // the way this works is that it needs to have that fuckin input string be Exactly. the same.
	if (regex_match(span, sm, s))
	{
		t = sm[4].str(); // the inner html

		if (incl(sm[2].str())) // if it's an important span, held in the static thing, then we also add the tags
		{
			t = r.printTag() + t + r.printClose();
		}
	}

	return t;
}
bool li::incl(string k) // k for class
{
	bool t{false};
	// anyway search through the thing
	for (cssRule rule : cssRule::stylesheet)
	{
		if (rule.klass == k)
		{
			t = true;
			r = rule;
			break;
		}
	}
	return t;
}

ostream &operator<<(ostream &os, const li &l)
{
	os << l.clean;
	return os;
}


/* ruby */

/* operator overloading */
sanitize &operator<<(sanitize &san, const ruby &r) {
	// san.innerHTML
	return san;
}