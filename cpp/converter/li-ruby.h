#ifndef LI_RUBY
#define LI_RUBY

#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>
#include "whatever.h" // has dependencies on cssRule and sanitize

using namespace std;

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

ostream &operator<<(ostream &, const li &);

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