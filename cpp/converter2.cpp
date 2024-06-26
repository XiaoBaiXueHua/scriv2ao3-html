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
bool batch{false}, hasDirectory{false}, recurve{false};
const filesystem::path fol({"html"});
filesystem::path currPath({fol});				  // initialize to just be the html folder
vector<filesystem::directory_entry> entries = {}; // want this to be of directory entries so that we can check if it's a directory later
sClean scriv;
void showEntries(filesystem::path);
void showOpts()
{
	project::center(50, "~~~ Options ~~~ ");
	cout << "\t1. Convert all files (but not sub-folders)." << endl;
	cout << "\t2. Convert all files and sub-folder files." << endl;
	cout << "\t3. Convert only some of the files shown." << endl;
	if (hasDirectory)
	{
		cout << "\t4. Look through one of the sub-folders listed." << endl;
	}
	cout << "\t> ";
}
void explorer()
{
	int dirNav{0};
	bool chosenOpt{false};
	while (!chosenOpt)
	{
		if (cin.good())
		{
			if (convertOpt == dirOpt)
			{
				cout << "Choose the directory to navigate to: ";
				cin >> dirNav;
				if (cin.good() && (dirNav < entries.size() || dirNav > 0))
				{
					currPath = entries[dirNav - 1];
				}
				showEntries(currPath);
				showOpts();
				// cin >> convertOpt;
			}
			else
			{
				chosenOpt = true;
				// if (cin.bad())
				// {
				// 	cin.clear();			 // clear the error flag
				// 	cin.ignore(10000, '\n'); // and then ignore the everything
				// }
				// cout << "Um. Try that again. ";
				break;
			}
		}
		else
		{							 // if the input is NOT good
			cin.clear();			 // clear error flag
			cin.ignore(10000, '\n'); // ignore the everything
			cout << "Um. You mistyped something. Try that again: ";
			// explorer(); // keep doing it until it's good
		}
		cin >> convertOpt;
	};
	// if (!cin.good())
	// {

	// }
}

int main()
{

	project::title("      HTML Sanitizer      ");
	if (!fol.empty())
	{
		project::center(25, "~~~ Files & Directories Available for Conversion ~~~");
		showEntries(fol);
		showOpts();
		cin >> convertOpt;
		explorer();
		cout << "Would you like to consolidate all these files to a single output? \n\t1. Yes\n\t2. No\n\t> ";
		switch (convertOpt)
		{
		case 1:
		{ // convert all files but not sub-folders
			break;
		}
		case 2:
		{ // convert all files and subfolders
			recurve = true;
			break;
		}
		case 3:
		{ // convert only some of the files shown
			cout << "Choose the files to be converted: ";
			// probably write a function for this
			break;
		}
		}
		// if (cin.good())
		// {

		// }
		// else
		// {
		// 	cout << "Um. You typed something wrong." << endl;
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
	entries = {}; // reset these each time
	numFiles = 0;
	hasDirectory = false;
	cout << endl; // add in an extra gap line for readability
	if (!p.empty())
	{
		for (auto const &dir_entry : filesystem::directory_iterator{p}) // list out the available options
		{
			filesystem::path currPath{dir_entry.path()};
			if (dir_entry.is_directory() || regex_search(currPath.extension().string(), regex("html"))) // only includes html files or directories
			{
				if (dir_entry.is_directory())
				{
					hasDirectory = true;
				}
				entries.push_back(dir_entry);
				numFiles++;
				cout << setw(15) << numFiles << ". " << currPath.filename() << (dir_entry.is_directory() ? " (directory)" : "") << endl;
			}
		}
	}
	else
	{
		cout << "Hey!! This thing's empty!!!!!!!!!!!!" << endl;
	}
}