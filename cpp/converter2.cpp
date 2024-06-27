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
bool batch{false}, hasDirectory{false}, chosenOpt{false};
const filesystem::path fol({"html"});
filesystem::path currPath({fol});				  // initialize to just be the html folder
vector<filesystem::directory_entry> entries = {}; // want this to be of directory entries so that we can check if it's a directory later
sClean scriv;
void showEntries(filesystem::path);
void showOpts()
{
	project::center(50, "~~~ Options ~~~ ");
	std::cout << "\t1. Convert all files (but not sub-folders)." << endl;
	std::cout << "\t2. Convert all files and sub-folder files." << endl;
	std::cout << "\t3. Convert only some of the files shown." << endl;
	if (hasDirectory)
	{
		std::cout << "\t4. Look through one of the sub-folders listed." << endl;
	}
	std::cout << "\t> ";
}
void typo()
{
	std::cin.clear();			 // clear error flag
	std::cin.ignore(10000, '\n'); // ignore the everything
	std::cout << "Um. You mistyped something. Try that again: ";
}
void explorer()
{
	int dirNav{0};
	while (!chosenOpt)
	{
		if (std::cin.good())
		{
			if (convertOpt == dirOpt)
			{
				std::cout << "Choose the directory to navigate to: ";
				std::cin >> dirNav;
				if (std::cin.good() && (dirNav < entries.size() || dirNav > 0))
				{
					currPath = entries[dirNav - 1];
				}
				showEntries(currPath);
				showOpts();
				// std::cin >> convertOpt;
			}
			else
			{
				// chosenOpt = true;
				break;
			}
		}
		else
		{							 // if the input is NOT good
			typo();
		}
		std::cin >> convertOpt;
	};
}

int main()
{
	vector<filesystem::directory_entry> selectedFiles;
	project::title("      HTML Sanitizer      ");
	std::cout << endl; // extra line for cleanliness
	if (!fol.empty())
	{
		int miscChoices{0};
		project::center(25, "~~~ Files & Directories Available for Conversion ~~~");
		showEntries(fol);
		showOpts();
		std::cin >> convertOpt;
		explorer();

		switch (convertOpt)
		{
		case 1:
		{ // convert all files but not sub-folders
			for (auto const &dir_entry : entries)
			{
				if (!dir_entry.is_directory())
				{

					selectedFiles.push_back(dir_entry);
				}
			}
			break;
		}
		case 2:
		{ // convert all files and subfolders
			// recurve = true;
			for (auto const &dir_entry : entries)
			{
				if (dir_entry.is_directory())
				{
					for (auto const &sub_entry : filesystem::recursive_directory_iterator{dir_entry})
					{
						selectedFiles.push_back(sub_entry);
					}
				}
				else
				{
					selectedFiles.push_back(dir_entry);
				}
			}
			break;
		}
		case 3:
		{ // convert only some of the files shown
			std::cout << "(Entering selection loop. Enter -1 to exit.)" << endl;
			std::cout << "Choose the files to be converted:\n\t> ";
			std::cin >> miscChoices;
			// probably write a function for this
			while ((!chosenOpt && (selectedFiles.size() < entries.size()) || miscChoices == -1))
			{
				if (std::cin.good())
				{
					if (miscChoices == -1) {
						break; // exit upon entering -1
					}
					selectedFiles.push_back(entries[miscChoices-1]);
					std::cout << "Added " << entries[miscChoices-1] << ".";
					std::cout << "\n\t> ";
				}
				else 
				{
					typo();
				}
				std::cin >> miscChoices;
			}
			break;
		}
		}
		miscChoices = 0; // reset this
		std::cout << "Would you like to consolidate all these files to a single output? \n\t1. Yes\n\t2. No\n\t> ";
		bool consolidate{false};
		std::cin >> miscChoices;
		while (!chosenOpt)
		{ // chosenOpt has now officially become the boolean i use for my misc while loops to ensure that the inputs are good
			if (std::cin.good())
			{
				if (miscChoices == 1 || miscChoices == 2)
				{
					consolidate = (miscChoices == 1);
					break;
				}
				else
				{
					std::cout << "That's not an option. Do it again: ";
				}
			}
			else
			{
				// std::cout << "TYPOOOOOOOOOOOO. pls retype: ";
				typo();
			}
			std::cin >> miscChoices;
		}
		for (auto const &dir_entry : selectedFiles) {
			// now we convert them one by one
			const filesystem::path currPath{dir_entry.path()};
			scriv.setBatch(consolidate);
			scriv.setName(currPath.stem().string());
			// scriv.setFullPath(currPath.string());
			std::cout << "Now cleaning: " << currPath << "." << endl;
			scriv.executor();
		}
	}
	else
	{
		std::cout << "Uh oh! There seems to be nothing in the " << fol << " folder. Twagic. We are hard-coded to only look for files in there.";
	}
	std::cout << "\nOkieee bye-bye ^w^";
	return 0;
}

void showEntries(filesystem::path p)
{
	entries = {}; // reset these each time
	numFiles = 0;
	hasDirectory = false;
	std::cout << endl; // add in an extra gap line for readability
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
				std::cout << setw(15) << numFiles << ". " << currPath.filename() << (dir_entry.is_directory() ? " (directory)" : "") << endl;
			}
		}
	}
	else
	{
		std::cout << "Hey!! This thing's empty!!!!!!!!!!!!" << endl;
	}
}