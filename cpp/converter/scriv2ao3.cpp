#include <iostream>
#include <iomanip>
#include <filesystem>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <regex>
#include <algorithm>
#include <cmath>
#include "options.h"
#include "whatever.h" // has cssRule and sanitize
#include "whatever.cpp"
#include "li-ruby.h"
#include "li-ruby.cpp"
#include "friends.h"
#include "friends.cpp"
#include "cleaner.h"
#include "cleaner.cpp"

using namespace std;

stringstream sstr, sstr2;
filesystem::directory_entry currFile{options::htmlFolder}; // current file, initialized to the html folder

vector<filesystem::directory_entry> entries; // the vector to help us navigate i guess! and also the entries thing for displaying the stuff
string tmp{""}, tmp2{""}, tmp3{""};
int convertOpt{0};

void explorer();
void resetEntries();
void resetEntries(bool);
void showEntries();
void configure();

int main()
{
	configure(); // configure first n foremost

	// now that we're out, see if we can make any determinations abt convertOpt
	if (options::navigator && convertOpt == 0)
	{
		// so if we have the recursive/consolidation settings, like, set, then we should set convertOpt based on said settings
		if (options::batch && options::recursive)
		{
			convertOpt = 2;
		}
		else if (options::batch)
		{
			convertOpt = 1;
		}
	}

	// pass along our static vars
	sanitize::prettify = options::prettify;
	sanitize::fill = options::setf;
	sanitize::hrStr = options::hr;

	// cleaned << setfill(options::setf); // set this to tabs

	if (!options::configured || options::navigator)
	{
		// if it's not been configured, then naturally we will show the entries n options n stuff
		showEntries();
		explorer();
		showMaze();
	}

	// cout << "Current File is: " << currFile << endl;
	// convertOpt = 3;
	switch (convertOpt)
	{
	case 1:
	{ // all files, but not sub-folders
		cout << "Cleaning all files, but not sub-folders." << endl;
		// resetCurrentPath(currFile); // since we're not doing sub-folders, this is enough
		for (auto const &dir_entry : filesystem::directory_iterator{currFile})
		{
			// currFile = dir_entry;
			cleaner o(dir_entry);
		}
		break;
	}
	case 2:
	{
		// all files AND sub-folders
		cout << "Cleaning all files And sub-folders." << endl;
		for (auto const &dir_entry : filesystem::recursive_directory_iterator{currFile})
		{

			// resetCurrentPath(dir_entry); // this should be redone every time
			if (!dir_entry.is_directory())
			{
				//
				// currFile = dir_entry;
				cleaner o(dir_entry);
			}
			else
			{
				cout << dir_entry << " is a folder." << endl;
			}
		}
		break;
	}
	case 3:
	{
		resetEntries();
		// only some files
		cout << "Choose a file or folder: ";
		cin >> convertOpt;
		while ((convertOpt < 1 || convertOpt > entries.size()))
		{
			// make sure we've chosen an Actual Thing
			cout << "Not Allowed. Try again: ";
			cin >> convertOpt;
		}

		currFile = entries[convertOpt - 1];
		if (currFile.is_directory())
		{
			// loop all
			// resetCurrentPath(currFile);
			for (auto const &dir_entry : filesystem::directory_iterator{currFile})
			{
				// currFile = dir_entry;
				cleaner o(dir_entry);
			}
		}
		else
		{
			// do the one
			cleaner o(currFile);
		}
		// if the selected thing
		break;
	}
	}
	return 0;
}

/* NAVIGATIONAL  */
void explorer() // transforms convertOpt
{
	int dirNav{0};
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
					if (dirNav > -1)
					{
						currFile = entries[dirNav - 1];
						options::fileMaze.push_back(currFile.path().stem().string());
					}
					else
					{
						cout << "Well, now you're SUPPOSED to be going back up a file." << endl;
						options::fileMaze.pop_back(); // otherwise remove the current directory from the file maze
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
					// tbh just let it transform convertOpt n head back into main
					cout << "hoho middle option " << convertOpt << endl;
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
void resetEntries()
{
	resetEntries(false); // default is verbose
}
void resetEntries(bool silence)
{
	if (!silence)
	{
		cout << "currFile: " << currFile << endl;
	}
	entries.clear();			   // start by clearing out the vector for display things
	options::hasDirectory = false; // reset this
	int numFiles{0};
	for (auto const &dir_entry : filesystem::directory_iterator{currFile})
	{
		filesystem::path currPath{dir_entry.path()};
		if ((dir_entry.is_directory() && !regex_search(currPath.string(), regex("_files$"))) || regex_search(currPath.extension().string(), regex("html"))) // only includes html files or directories
		{
			if (dir_entry.is_directory())
			{
				options::hasDirectory = true;
			}
			entries.push_back(dir_entry);
			numFiles++;
			if (!silence)
			{
				cout << setw(15) << numFiles << ". " << currPath.filename() << (dir_entry.is_directory() ? " (directory)" : "") << endl; // print this if not supposed to be silent
			}
		}
	}
}
void showEntries()
{
	resetEntries();
	if (!currFile.path().empty() && entries.size() > 0)
	{
		// project::center(50, "~~~ Options ~~~ ");
		cout << "\t1. Convert all files (but not sub-folders)." << endl;
		cout << "\t2. Convert all files and sub-folder files." << endl;
		cout << "\t3. Convert only some of the files shown." << endl;
		if (options::hasDirectory)
		{
			cout << "\t4. Look through one of the sub-folders listed." << endl;
		}

		// cout << "\t4. " << (hasDirectory ? "Look through one of the sub-folders listed." : "Go back to the previous folder.") << endl;
		cout << "\t> ";
	}
	else
	{
		cout << "Hey!! This thing's empty!!!!!!!!!!!!!!!!" << endl;
		exit(0);
	}
}
void configure()
{
	fstream c("scriv2ao3.config");
	if (c.is_open())
	{
		// if it opens
		cout << "...configuring..." << endl;
		resetEntries(true); // silently initialize the entry vector
		options::configured = true;
		vector<string> config = {};
		while (getline(c, tmp))
		{
			// get each line
			if (tmp != "" && tmp[0] != '#') // so if it's not blank n it doesn't start w/a #
			{
				sstr << tmp;
				while (getline(sstr, tmp2, '='))
				{
					config.push_back(trim(tmp2)); // trim it just to make sure. in case someone else wants to have spaces for some goddamn reason
				}
				string uc{tupper(config[0])}; // only run this once per loop

				if (uc == "RECURSIVE" || uc == "BATCH" || uc == "OPT")
				{
					// if either of these are turned on, then that means the user probably knows what they want n don't need to be asked what option they'd like
					options::navigator = false;
				}
				// now analyze the thing
				if (uc == "SRC")
				{
					options::htmlFolder = config[1];
					currFile = filesystem::directory_entry(options::htmlFolder); // set this as well
				}
				else if (uc == "PRETTIFY")
				{
					options::prettify = tf(config[1]);
				}
				else if (uc == "SETFILL")
				{
					// no matter what, will only take the first char
					if (config[1] == "tab")
					{
						options::setf = '\t';
					}
					else if (config[1] == "space")
					{
						options::setf = ' ';
					}
					else
					{
						options::setf = char(config[1][0]); // otherwise, just take the first char of the string
					}
				}
				else if (uc == "BATCH")
				{
					options::batch = tf(config[1]);
				}
				else if (uc == "RECURSIVE")
				{
					options::recursive = tf(config[1]);
				}
				else if (uc == "CONSOLIDATE")
				{
					options::consolidate = tf(config[1]);
				}
				else if (uc == "DELETESRC")
				{
					options::deletesrc = tf(config[1]);
				}
				else if (uc == "COPYSRC")
				{
					options::copysrc = tf(config[1]);
				}
				else if (uc == "COPYFOLDER")
				{
					options::copyFolder = config[1];
				}
				else if (uc == "HRSTR")
				{
					options::hr = config[1];
				}
				else if (uc == "OPT")
				{
					// in this one they just straight up choose which option
					convertOpt = stoi(config[1]);
				}
				else if (uc == "OUTPUTFOLDER")
				{
					options::outputFolder = config[1];
				}
				else if (uc == "PROCESSRUBY")
				{
					ruby::process = tf(config[1]);
				}
				else if (uc == "RUBYREGEX")
				{
					// options::rubyregex = regex_replace(config[1], regex("\\"), "\\\\");
					ruby::rubyregex = config[1]; // trying to do a regex_replace here causes a crash
				}
				else if (uc == "RUBYSPLIT")
				{
					string ucr{tupper(config[1])};
					if (regex_search(ucr, regex("^\\d+$")))
					{
						// so if it's nothing but numbers
						ruby::splitOpt = stoi(ucr); // no we're not going to verify that shit. that's the user's problem.
					}
					else
					{
						if (ucr == "AUTO")
						{
							ruby::splitOpt = 1;
						}
						else if (ucr == "ASK")
						{
							ruby::splitOpt = 2;
						}
						else if (ucr == "LETTER")
						{
							ruby::splitOpt = 3;
						}
						else if (ucr == "WORD")
						{
							ruby::splitOpt = 4;
						}
						else if (ucr == "NOSPLIT")
						{
							ruby::splitOpt = 5;
						}
					}
				}
				else if (uc == "RUBYINDEX")
				{
					// string ints{config[1]}; // make a string of the current integers so we can clear config
					// cout << tmp << endl;
					vector<int> ints = {};
					// config.clear();
					sstr.clear();
					sstr << config[1];
					cout << sstr2.str() << endl;
					while (getline(sstr, tmp2, ','))
					{
						ints.push_back(stoi(tmp2)); // new comma separated values
					}
					ruby::weh = make_pair(ints[0], ints[1]); // make a new pair out of the config strings :3
					sstr.clear();
				}

				sstr.clear(); // clear it at the end
				config.clear();
			}
		}

		cout << "config complete." << endl;
	}
	else
	{
		cout << "configuration failed." << endl;
	}
	c.close(); // and then close the stream ofc
	c.clear();
}
//