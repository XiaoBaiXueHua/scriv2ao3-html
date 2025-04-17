// fucking file for options now too i guess. god whatever!!
using namespace std;
#ifndef OPTS
#define OPTS
class options
{
public:
	static bool configured, prettify, navigator, recursive, consolidate, copysrc, deletesrc, batch, hasDirectory;
	static char setf;
	static unsigned int tabMultiplicity;
	static string htmlFolder, outputFolder, copyFolder, hr;
	static vector<string> fileMaze;
};

bool options::configured{false}, options::prettify{true}, options::navigator{true}, options::recursive{false}, options::consolidate{false}, options::hasDirectory{false}, options::copysrc{false}, options::deletesrc{false}, options::batch{false};

char options::setf('\t');

unsigned int options::tabMultiplicity{1};

string options::htmlFolder{"html"}, options::outputFolder{"output"}, options::copyFolder{"converted"}, options::hr{"~***~"};

vector<string> options::fileMaze = {};
#endif