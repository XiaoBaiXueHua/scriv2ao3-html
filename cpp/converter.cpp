#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
// #include <iterator>
// #include <regex>
// #include <chrono>
// #include <ctime>
#include <filesystem>
#include "subfiles/styler.h"
#include "subfiles/styler.cpp"
#include "subfiles/jkp_std.h"

using namespace std;

int main()
{
	char ans{'y'}, changeDir{'n'}, instr{'n'};
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
		if (opt == 2) {
			scriv.setBatch(true);
			// std::cout << "Please enter the name of the folder you would like to convert: ";
			// std::cin >> dir;
			// const filesystem::path fol{dir};
			cout << "setting fol to \"html/cazzo\"" << endl;
			const filesystem::path fol({"html\\cazzo"});
			std::cout << "The files available: " << endl;
			int numFiles{0};
			for (auto const& dir_entry : filesystem::directory_iterator{fol}) {
				std::cout << setw(15) << numFiles << ". " << dir_entry.path() << endl;
				numFiles++;
			}
			cout << "Would you like to:\n\t1. Convert each of these files into their own separate, cleaned files?\n\t2. Merge each of these files into one cleaned html file?\n\t3. Only convert one (or some) of these files? ";
			cin >> opt;
			if (opt == 2) {
				cout << "Would you like to reorder them from the order presented? [Y/y]: ";
			}
			break;
		} else {
			scriv.setBatch(false);
			scriv.inputName(); // better to just have this outside the execution tbh
			if (gdoc) {
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
		std::cout << "Enter Y/y to convert another file, or else hit any key to exit: ";
		std::cin >> ans;

		if (toupper(ans) == 'Y')
		{
			std::cout << "\n\nWould you like to change the source folder for the html file? ";
			std::cin >> changeDir;
			if (toupper(changeDir) == 'Y')
			{
				std::cout << "\nPlease specify the name of the file folder: ";
				std::cin >> dir;
				scriv.setiPath(dir);
				std::cout << endl;
			}
		}
	}
	return 0;
}
