#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <iterator>
#include <regex>

using namespace std;

fstream scriv; // first declare the streams we'll be needing
string filename{""};
void findStyle(fstream &); // okay so. we pass these along by reference, got it

int main()
{
	// system("Notepad.exe");
	cout << "Choose the name of the html file to open: ";
	cin >> filename;
	// fstream scriv ("html/cazzo.html");
	scriv.open("private/" + filename + ".html"); //later change the hard-coding of "private" to just html
	if (scriv.is_open())
	{
		fstream ao3; // once we can get the scriv file open, now we can open Another fstream for ao3
		findStyle(ao3);
		scriv.close();
	}
	else
	{
		cout << "Hmmm yeah i think these might have to be pre-existing files. Which makes sense. But yeah unable to open file.\n";
		exit(0);
	}
	// scriv.open("private/hi.html");
	// scriv << "y'know, like, nya~" << endl;
	// scriv.close();

	return 0;
}

void findStyle(fstream &html)
{
	// fstream html;
	// html.clear(); //clear it
	// html.
	// clear(html);
	int numLines{30}; // initialize to 30 for no real reason. just funsies.
	cout << "\nPick how many lines you would like to view: ";
	cin >> numLines;
	cout << "\n\nAnd now, we shall print the first " << numLines << " lines of the file.\n\n\n"
		 << setw(30) << " "
		 << "*****\n\n\n";
	string fpath{"output/" + filename + ".html"};
	html.open(fpath); // for now, there should be an existing file of the same name in the output folder, but later can make it so that it makes it all on its own

	if (!html.is_open())
	{
		html.clear();
		html.open(fpath, ios::out); // if the file doesn't already exist, make it
		html.close(); //close it
		html.open(fpath); //and then reopen it correctly
	}
	string line;
	regex styleStart("<style type=\"text/css\">");
	bool style{false};

	for (int i{1}; i <= numLines; i++)
	{
		getline(scriv, line);
		if (regex_search(line, styleStart))
		{ // if the style element has begun, then set the style boolean to true so that we can start outputting the lines
			style = true;
		}
		if (style)
		{
			html << line << endl;
		}
		if (scriv.eof() || regex_search(line, regex("</style>")))
		{ // || regex_search(line, regex("</style>"))
			numLines = i;
			break; // if we reach the end of the file, end the loop early
		}

		// cout << getline(scriv, i) << "\n";
	}

	cout << "We duplicated the first " << numLines << " lines into the output! :D" << endl;
	html.close();
}
// regex findEl(string el, bool endEl) {
// 	//string newEl{"<" + (bool(endEl) ? "/" : "") + el + ">"};
// 	string pat{'<'+(endEl ? "/" : "") + el + '>';
// 	//return new regex(pat);
// }