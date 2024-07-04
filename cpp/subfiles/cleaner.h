#ifndef SCRIV_SANITIZER
#define SCRIV_SANITIZER
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <cctype>
#include <locale>
#include <regex>
#include <filesystem>
#include "./jkp_std.h"

using namespace std;

class sClean
{
public:
	sClean(){}; // default constructor
	~sClean()
	{
		raw.close();
		cleaned.close();
	}

	void showMaze()
	{
		cout << "The maze navigation looks like: " << endl;
		for (auto const &entry : fileMaze)
		{
			cout << entry << " / ";
		}
	}
	string currentPath()
	{ // returns the maze up to the current folder
		string tmp{""};
		for (auto const &entry : fileMaze)
		{
			tmp += entry + " / ";
		}
		return tmp;
	}
	void explorer()
	{
		int convertOpt{0}, dirNav{0};
		while (true)
		{ // it runs until you get it right what more could you want
			cin >> convertOpt;
			if (cin.good())
			{
				if (convertOpt == 4)
				{
					cout << "Choose the directory to navigate to, or enter -1 to go back: ";
					cin >> dirNav;
					if (cin.good())
					{
						if (dirNav != -1)
						{
							currFile = entries[dirNav - 1];
							fileMaze.push_back(currFile.path().stem().string());
						}
						else
						{
							cout << "Well, now you're SUPPOSED to be going back up a file." << endl;
							fileMaze.pop_back(); // otherwise remove the current directory from the file maze
							try
							{
								currFile = filesystem::directory_entry{currFile.path().parent_path()};
							}
							catch (exception)
							{
								cout << "Yeah whatever just got tried for navigating up in the file path didn't work. but rest assured that the filemaze got correctly done or something." << endl;
							}
						}
					}
					showEntries(); // and then display the thing
				}
				else
				{
					if (convertOpt > 0 && convertOpt < 4)
					{
						break;
					}
					else
					{
						cout << "Ummmm that's not an option ://////\nTry again: ";
					}
				}
			}
			else
			{
				cout << "TYPO SPOTTED!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\npls try again: ";
			}
		}
	}
	void showEntries()
	{
		entries.clear(); // start by clearing out the vector for display things
		int numFiles{0};
		bool hasDirectory{false};
		if (!currFile.path().empty())
		{
			for (auto const &dir_entry : filesystem::directory_iterator{currFile})
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
			project::center(50, "~~~ Options ~~~ ");
			std::cout << "\t1. Convert all files (but not sub-folders)." << endl;
			std::cout << "\t2. Convert all files and sub-folder files." << endl;
			std::cout << "\t3. Convert only some of the files shown." << endl;
			// if (hasDirectory)
			// {
			// 	std::cout << "\t4. Look through one of the sub-folders listed." << endl;
			// }
			cout << "\t4. " << (hasDirectory ? "Look through one of the sub-folders listed." : "Go back to the previous folder.") << endl;
			std::cout << "\t> ";
		}
		else
		{
			cout << "Hey!! This thing's empty!!!!!!!!!!!!!!!!" << endl;
		}
	}

private:
	bool bulk{false};
	fstream raw, cleaned;						  // input/output streams
	filesystem::directory_entry currFile{"html"}; // current file, initialized to the html folder
	vector<filesystem::directory_entry> entries;  // the vector to help us navigate i guess! and also the entries thing for displaying the stuff
	vector<string> fileMaze;
};

#endif // end of SCRIV_SANITIZER