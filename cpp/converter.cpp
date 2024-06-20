#ifndef CONVERTER_DEPENDENCIES
#define CONVERTER_DEPENDENCIES
#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <algorithm>
#include <cctype> // these were used in the trim example on stackoverflow
#include <locale> // these were used in the trim example on stackoverflow
#include <iterator>
#include <regex>
#include <chrono>
#include <cstdlib>
// #include <chrono>
// #include <ctime>
#endif // end of CONVERTER_DEPENDENCIES
#include <filesystem>
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"
#include "subfiles/jkp_std.h"

using namespace std;

int main()
{
	char ans{'y'}, reorder{'n'}, instr{'n'};
	bool showIns{true}, gdoc{false};
	int opt{0};
	string dir{""};
	if (!showIns)
	{
		showIns = true;
		std::cout << "Enter Y/y to view instructions/documentation(?), or else hit any key to continue. ";
		std::cin >> instr;
		if (toupper(instr) == 'Y')
		{
			std::cout << "I'll figure out how to use the .exe file on its own after compiling later. But for now, you need to have at least one folder called \"html\" in the same folder that has this .cpp file.\nYou can have other folders with other html files that can be cleaned and run after you sanitize at least one (1) html file from the \"html\" folder. This is because that's how I hard-coded it :YunJinWink:\nAnyway, you also need to have a folder called \"output\" for your outputs bc I haven't bothered to integrate code that will make a directory if one doesn't yet exist yet because I'm lazy and this project is For Me, Personally, first and foremost.\n\n";
			std::cout << "Anyway. If you're reading this, you can probably read .cpp files yourself, but I guess to be clear, the `executor()` function is the one that, like, does all the steps of the cleaning. It's at the very bottom of \"styler.cpp\" because even though I am not very good at keeping my code organized as it grows (the README in the \"converter\" folder is arguably the pseudocode. I made no UML bc I've been avoiding that in class like a coward.), I figured that at the very least, I should keep that there at the bottom.\nFunctions are at least groups with their overloads, though they're not organized by the sorts of things they do or even in the same order in the \".cpp\" file as the \".h\" file.\n`findStyle()` and `sanitize()` are, at the very least, kept together. Near the top, far far away from the `executor()` function that uses them.\nI forgot where I was going with this and why I was doing it. But hey! At least we have a logger function uwu.\n\n";
		}
	}
	sClean scriv;
	project::title("HTML Sanitizer");

	fstream r;
	sClean::open(r, "subfiles/rules.csv");
	scriv.ruleInit(r);
	// vector<string>rules{"text-align", "margin-left", "text-decoration"}, cssVals{""};
	// this unnests the gdoc or other minified html inputs
	// scriv.setRaw("html/sffp.html");
	// scriv.setClean("tmp/sffp_tmp.html");
	// scriv.nester();

	// main loop
	while (toupper(ans) == 'Y')
	{
		project::center(50, "~~~ Options ~~");
		std::cout << setw(5) << "1" << ". Convert a single HTML file." << endl;
		std::cout << setw(5) << "2" << ". Convert a folder of HTML files.\n"
				  << endl;
		std::cout << "Please choose an option: ";
		cin >> opt;
		// test to make sure what was input was actually a fucking number
		if (opt == 2)
		{
			scriv.setBatch(true);
			// std::cout << "Please enter the name of the folder you would like to convert: ";
			// std::cin >> dir;
			// const filesystem::path fol{dir};
			cout << "setting fol to \"html/cazzo\"" << endl;
			const filesystem::path fol({"html"});
			std::cout << "The files available: \nIn the root " << fol << " folder: " << endl;
			int numFiles{0};
			vector<filesystem::path> files = {}, dirs = {};
			// for (auto const &dir_entry : filesystem::recursive_directory_iterator{fol})
			for (auto const &dir_entry : filesystem::directory_iterator{fol})
			{
				// dir_entry.is_directory() ? dirs.push_back(dir_entry.path()) : files.push_back(dir_entry.path());
				filesystem::path currPath{dir_entry.path()};
				if (dir_entry.is_directory())
				{
					std::cout << "In the " << currPath.filename() << " folder, we have:\n";
				}
				else if (regex_search(currPath.extension().string(), regex("html")))
				{
					// std::cout << "currPath.extension().string().substr(1, currPath.extension().string().length())" << currPath.extension().string().substr(1, currPath.extension().string().length()) << endl;
					// string fp{currPath.extension().string()}; // gotta declare it as a string i guess
					// if (regex_search(fp, regex("html"))) {
					// 	// 
					// }
					numFiles++;
					std::cout << setw(15) << numFiles << ". " << currPath.filename() << endl;
					// std::cout << "(current path extension is " << currPath.extension() << ")";
				}
				//
				// std::cout << setw(15) << numFiles << ". " << dir_entry.path() << ", is directory? " << dir_entry.is_directory() << endl;
			}
			// for (auto const& file : files) {
			// 	std::cout << setw(15) << numFiles << ". " << file.filename();
			// }
			cout << "Would you like to:\n\t1. Convert each of these files into their own separate, cleaned files?\n\t2. Merge each of these files into one cleaned html file?\n\t3. Exit the program?" << endl;
			cin >> opt;
			if (opt == 2)
			{
				vector<int> order = {};
				int num{0};
				cout << "Would you like to reorder them from the order presented? Enter [Y/y] if so: ";
				cin >> reorder;
				if (toupper(reorder) == 'Y')
				{
					cout << "Please enter the order you would like the files to be in, by the numbers they were assigned earlier: " << endl;
					;
					for (int i{0}; i < numFiles; i++)
					{
						cout << "\t" << i + 1 << ". ";
						cin >> num;
						cout << i + 1 << ". " << files[num - 1] << endl;
						order.push_back(num);
					}
				}
			}
			else if (opt == 1)
			{
				for (auto const &dir_entry : files)
				{
					// string path{dir_entry.path().string()};
					// string file = regex_replace(dir_entry.string(), scriv.toRegEx(dir), "");
					scriv.setFullPath(dir_entry.string());
					cout << "Now cleaning " << dir_entry.string() << endl;
					scriv.executor();
					// cout << "file to be cleaned: " << file << endl;
				}
			}
			else if (opt == 3)
			{
				exit(0);
			}
			break;
		}
		else
		{
			scriv.setBatch(false);
			scriv.inputName(); // better to just have this outside the execution tbh
			if (gdoc)
			{
				scriv.gClean();
				// scriv.setType("html");
				// scriv.setRaw(scriv.getFullPath(true));
				// scriv.setClean(scriv.getTmpPath());
				// string temp;

				// scriv.unminify();
				// and then swap the streams and replace the original raw w/the unminified version
				// break;
			}
			scriv.executor();
		}
		std::cout << "Enter Y/y to convert more file(s), or else hit any key to exit: ";
		std::cin >> ans;

		// if (toupper(ans) == 'Y')
		// {
		// 	std::cout << "\n\nWould you like to change the source folder for the html file? ";
		// 	std::cin >> changeDir;
		// 	if (toupper(changeDir) == 'Y')
		// 	{
		// 		std::cout << "\nPlease specify the name of the file folder: ";
		// 		std::cin >> dir;
		// 		scriv.setiPath(dir);
		// 		std::cout << endl;
		// 	}
		// }
	}
	r.close(); // don't forget to close the rules lol
	return 0;
}
