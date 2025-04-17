#ifndef LI_CLEANS
#define LI_CLEANS
#include <iostream>
#include <string>
#include <vector>
#include <regex>
#include <sstream>

using namespace std;

class li // actually i think it might be time to make a class for this one.
{
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

// ruby class got moved to whatever.h to work with sanitize :<
// its functions will probably be staying in li-ruby.cpp anyway tho

#endif