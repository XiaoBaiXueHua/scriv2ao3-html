#include <iostream>
#include <iomanip>
// #include <fstream>
#include <string>
#include <vector>
// #include <algorithm>
// #include <iterator>
#include <regex>
#include <filesystem>
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"
#include "subfiles/jkp_std.h"

using namespace std;

// global vars and stuff
int dirOpt{4}, convertOpt{dirOpt}, numFiles{0};
bool batch{false};
const filesystem::path fol({"html"});
filesystem::path currPath({fol});				  // initialize to just be the html folder
vector<filesystem::directory_entry> entries = {}; // want this to be of directory entries so that we can check if it's a directory later
sClean scriv;
void showEntries(filesystem::path);
void explorer()
{
	do
	{
		if (cin.good() && convertOpt > 0 && convertOpt < 5)
		{
			showEntries(currPath);
			project::center(50, "~~~ Options ~~~ ");
		}
		else
		{
			if (cin.bad())
			{
				cin.clear();			 // clear the error flag
				cin.ignore(10000, '\n'); // and then ignore the everything
			}
			cout << "Um. Try that again. ";
			cin >> convertOpt;
		}
	} while (convertOpt == dirOpt);
}
int main()
{

	project::title("      HTML Sanitizer      ");

	// project::center(50, "~~~ Options ~~~");
	// const filesystem::path fol({"html"});
	if (!fol.empty())
	{
		project::center(25, "~~~ Files & Directories Available for Conversion ~~~");
		showEntries(fol);
		// filesystem::directory_entry chosen{explorer(entries[convertOpt - 1])};
		// if (chosen.is_directory())
		// {
		// 	// bc explorer can return either a single file or a folder, we still have to check this
		// }
	}
	else
	{
		cout << "Uh oh! There seems to be nothing in the " << fol << " folder. Twagic. We are hard-coded to only look for files in there.";
	}
	cout << "\nOkieee bye-bye ^w^";
	return 0;
}

void showEntries(filesystem::path p)
{
	entries = {}; // reset this each time
	numFiles = 0;
	for (auto const &dir_entry : filesystem::directory_iterator{p}) // list out the available options
	{
		filesystem::path currPath{dir_entry.path()};
		if (dir_entry.is_directory() || regex_search(currPath.extension().string(), regex("html"))) // only includes html files or directories
		{
			entries.push_back(dir_entry);
			numFiles++;
			cout << setw(15) << numFiles << ". " << currPath.filename() << (dir_entry.is_directory() ? " (directory)" : "") << endl;
		}
	}
	// filesystem::directory_entry chosen{entries[convertOpt - 1]}; // has to be -1 bc the numbers are listed out + 1

	// explorer(chosen);
}