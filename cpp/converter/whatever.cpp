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
#include "whatever.h"

using namespace std;

fstream raw, cleaned; // input/output streams
stringstream sstr, sstr2;
filesystem::directory_entry currFile{"html"}; // current file, initialized to the html folder
vector<filesystem::directory_entry> entries;  // the vector to help us navigate i guess! and also the entries thing for displaying the stuff
vector<string> fileMaze{};
vector<cssRule> stylesheet;
vector<pair<string, cssRule>> prevEls;
string tmp{""}, tmp2{""}, tmp3{""};
string innerHTML{""};
string full{""}, guts{""};

stringstream cleanLine;

smatch smidge;
bool hasDirectory{false};
int convertOpt{0}, trimAway{0};

void explorer();
void showEntries();
void showMaze();
void cleaner();
pair<bool, cssRule> getRule(string &);
pair<bool, cssRule> getRule(string &, string &, string &);
void snip();
void handleLi();
string currentPath(); // returns the maze up to the current folder
void makeDir();
void open(filesystem::directory_entry); // opens up the cleaned thing

int main()
{
	// cout << "...hewwo???" << endl;
	showEntries();
	explorer();
	showMaze();
	// convertOpt = 3;
	switch (convertOpt)
	{
	case 1:
	{ // all files, but not sub-folders
		for (auto const &dir_entry : filesystem::directory_iterator{currentPath()})
		{
			if (!dir_entry.is_directory())
			{
				cleaner();
			}
		}
		break;
	}
	case 2:
	{
		// all files AND sub-folders
	}
	case 3:
	{
		// only some files
		cout << "Choose a file or folder: ";
		cin >> convertOpt;
		while ((convertOpt < 1 || convertOpt >= entries.size()))
		{
			// make sure we've chosen an Actual Thing
			cout << "Not Allowed. Try again: ";
			cin >> convertOpt;
		}
		currFile = entries[convertOpt - 1];
		if (currFile.is_directory())
		{
			// loop all
			for (auto const &dir_entry : filesystem::directory_iterator{currFile})
			{
				cleaner();
			}
		}
		else
		{
			// do the one
			cleaner();
		}
		// if the selected thing
		break;
	}
	}
	return 0;
}

void cleaner()
{
	cout << "Now cleaning: " << currFile.path().stem() << endl
		 << endl;
	raw.open(currFile.path()); // open the raw
	open(currFile);			   // open the output

	// bool listSwitch{false}; // bool for list mode
	bool styleSwitch{false}, bodySwitch{false}, bqtSwitch{false}, listSwitch{false};
	// vector<string> lines{}; // actually let's just have a vector of the body lines. i think that'll make it easier to work with in the end
	string prevEl{"p"};
	sanitize pls;

	// vector<pair<string, cssRule>> linear;
	vector<sanitize> linear;
	float medianFontSize{1.00};
	// vector<string> cssRules{};
	// cleaned.open("output/test.html"); // just the test for now
	cleaned.clear();
	// cleaned << "hi? " << endl;
	//
	stylesheet.clear();

	// go through the whole thing once
	while (getline(raw, tmp))
	{
		tmp = trim(tmp); // trim owo
		// cout << tmp << endl;
		// if (regex_search(tmp, regex("<style.+>")))

		if (styleSwitch)
		{
			// cssRules.clear();
			if (tmp == "</style>")
			{
				styleSwitch = false;
				cout << "end of style. now sorting the stylesheet by font size." << endl
					 << endl;
				// and then we should sort the stylesheet in various ways and process the vector to determine some of the other html things, like whether smth counts as <big> or <small>
				vector<cssRule> tmpStyles = stylesheet;
				stable_sort(tmpStyles.begin(), tmpStyles.end(), [](const cssRule &a, const cssRule &b)
							{
								return a.fontSize < b.fontSize; // sorts from least to greatest
							});
				// if the median font size is greater than 0, then set it to whatever that is
				if (tmpStyles[floor((tmpStyles.size() - 1) / 2)].fontSize > 0)
				{
					medianFontSize = tmpStyles[floor((tmpStyles.size() - 1) / 2)].fontSize;
				}
				// cout << "median font size: " << medianFontSize << endl;

				// cout << "ostensibly sorted by font size. let's check: " << endl;
				stylesheet.clear();
				// stylesheet.swap(&vector<cssRule>{});
				// stylesheet = {};
				for (auto r : tmpStyles)
				{
					if (r.fsSpecified)
					{ // this only matters if the font size has been specified
						// cout << "font size: " << r.fontSize << endl;

						if ((r.fontSize < medianFontSize - 0.1) || (r.fontSize > medianFontSize + 0.1))
						{
							// if the current element's font size isn't w/in 0.15rem of the median, then it gets considered <big> or <small>
							if (r.fontSize < medianFontSize - 0.1)
							{
								// if it's smaller...
								r.el = "small"; // turn it into a <small> element
							}
							else
							{
								r.el = "big";
							}
						}
					}
					// cout << "\t" << r.el << "." << r.klass << " {" << r.rulez << "}" << endl
					//  << endl;
					if (r.el != "span")
					{
						stylesheet.push_back(r);
					}
				}
			}
			else
			{
				cssRule ayo(tmp);
				// cout << "el: " << ayo.el << endl;
				if (ayo.worthwhile)
				{ // ignore the body type elements
					stylesheet.push_back(ayo);
				}
			}
		}
		if (bodySwitch)
		{

			if (tmp == "</body>")
			{
				bodySwitch = false;
				// cout << "complete." << endl;
				break;
			}

			cleanLine.clear();
			trimAway = 0; // integer to let us know the index of the proper substring

			bool prevClosing{false};
			regex anyEl("^(<(.*?)>)");
			regex spanner("(<span\\sclass=\"(\\w|\\d|\\-)+\">)([^<]+)?(</span>)"); // picks the first span only

			int nested{0}; // integer to track how many nested layers deep we are
			while (tmp != "")
			{
				regex_iterator elemental(tmp.begin(), tmp.end(), anyEl); // first one is the full element, the other is its guts; we mostly work based off what the guts are
				full = (*elemental)[0].str();
				guts = (*elemental)[2].str();
				// delete[] (*elemental);
				bool closing{guts[0] == '/'}; // boolean for if this is a closing element

				trimAway = full.length(); // default

				regex tagStructure("/?((\\w|\\d)+)(\\sclass=\"(.*?)\")?"); // this results in 0 = guts, 1 = element name, and 4 = class name

				regex_match(guts, smidge, tagStructure);
				string currentEl{smidge[1].str()};
				string currentClass{smidge[4].str()};
				// cout << "currentEl: " << currentEl << "\t\t" << "currentClass: " << currentClass << endl;
				pair<bool, cssRule> daRule = getRule(currentEl, currentClass, guts);
				cssRule relevant = daRule.second;
				bool endling{(currentEl == "p" && closing)}, block{relevant.display != "inline"}; // for now just closing paragraphs; will adjust as needed later

				if (block) // so. technically this just means "not an inline"
				{
					// cout << "not a span or li, right??? " << currentEl << endl;

					pls = sanitize("", cssRule(relevant.parent, currentClass, relevant.guts));
					// cout << "new pls: " << pls << endl
					// 	 << endl;
				}
				else if (!closing)
				{
					nested++;
				}
				if (currentEl == "li")
				{
					handleLi();
					tmp += "</li>"; // fuck it. just add itjust add it
					cout << tmp << endl;
					pls++;
					// listSwitch = false;
				}

				if (currentEl != "")
				{
					// cout << "\n\nCURRENT EL: " << currentEl << endl;
					if (closing)
					{
						if (!endling)
						{
							// cout << "prevEl: " << prevEl << endl;
							cleanLine << full;
						}
					}
					else
					{
						string sneefer{("(<" + currentEl + "(.*?)>)(.*?)(</" + currentEl + ">)")}; // the inner html will be index 3 for this

						// so if there is Anything relevant abt the class of the element we're currently working with
						regex snorf{sneefer}; // make a regex out of sneefer

						// this is p much the only way to get it to work on spans and p's for reasons that are frankly unknown to me
						sregex_iterator nyooo(tmp.begin(), tmp.end(), snorf);
						innerHTML = (*nyooo)[3].str();

						// cout << "innerHTML: " << innerHTML << endl;

						if (currentClass != "")
						{
							if (daRule.first && !block) // if it's an element w/particular styling rules
							{
								cleanLine << relevant.printTag(); // only print this if the previous element was the same as the current element...?
							}

							if (currentEl == "span")
							{
								// adjust the trim away bit
								trimAway = full.length() + innerHTML.length() + string("</" + currentEl + ">").length();

								cleanLine << innerHTML;
								if (daRule.first) // if we have a different replacement span
								{
									cleanLine << relevant.printClose();
								}
							}
							else if (currentEl == "p")
							{
								nested++;
							}
						}
						else
						{
							cleanLine << full;
						}

						if ((currentEl == "ul") || (currentEl == "ol"))
						{
							cout << "in a list" << endl;
							// listSwitch = true; // turn list mode on
							// cout << "listSwitch: true (" << listSwitch << ")" << endl;
							pls = sanitize("", cssRule(currentEl, currentClass, ""));
						}
					}
				}
				else
				{
					if (!closing) // shouldn't have to deal with this anymore, but we'll see still
					{
						cout << "...but smth weird happened.\n\t" << tmp << endl;
						if (tmp.length() <= 1)
						{
							tmp = "";
						}
						else if (!listSwitch)
						{
							cleanLine << full; // we'll fix this up more later when working with those wretched list items
						}
						else
						{
							break; // break it if we're in list mode
						}
					}
					// break;
				}
				snip();

				if (closing)
				{
					pls += cleanLine; // add the inner HTML to the pls
					// clear the line Before we break the thing
					stringstream whee("");
					cleanLine.swap(whee); // can we just. clear it like that.
					if (endling)		  // if tmp is empty or we're closing a paragraph
					{
						// cout << "this is an endling.\n\t" << currentEl << "\t" << currentClass << endl;
						linear.push_back(pls);
						pls.reset();
						break;
					}
				}

				// cout << "new tmp: " << tmp << endl;
			}
		}

		if (regex_search(tmp, regex("<style.+>")))
		{
			cout << "located the style." << endl;
			styleSwitch = true;
			// break;
		}

		if (regex_search(tmp, regex("<body.*>")))
		{
			// if we found the body
			cout << "now processing the body." << endl;
			bodySwitch = true;
		}
	}

	// now we go through the html vector with the glorious benefits of an index
	cout << "now to go through the lines array. (" << linear.size() << " lines)" << endl;
	for (int i{0}; i < linear.size(); i++)
	{
		bool more{i < linear.size() - 1}, hindsight{i > 0};
		sanitize prev, next, current{linear[i]};
		if (hindsight)
		{
			prev = linear[i - 1];
		}
		if (more)
		{
			next = linear[i + 1];
		}
		if (current.bqtMode)
		{
			// so if we're currently in blockquote mode
			if (!prev.bqtMode)
			{
				// default for bqtMode is false, so we don't need to worry abt doing some shit wrong
				cleaned << current.printParent() << endl;
			}
		}
		if (current.length() > 1)
		{
			// now we print our tabs
			cleaned << setfill('\t') << setw(current.length() - 1) << "";
		}

		cleaned << current << endl;

		if (current.bqtMode)
		{
			if (!next.bqtMode)
			{
				cleaned << current.closeParent() << endl;
			}
		}
	}

	raw.close(); // close the streaaaaams
	cleaned.close();
}

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

void showEntries()
{
	entries.clear();	  // start by clearing out the vector for display things
	hasDirectory = false; // reset this
	int numFiles{0};
	if (!currFile.path().empty())
	{
		for (auto const &dir_entry : filesystem::directory_iterator{currFile})
		{
			filesystem::path currPath{dir_entry.path()};
			if ((dir_entry.is_directory() && !regex_search(currPath.string(), regex("_files$"))) || regex_search(currPath.extension().string(), regex("html"))) // only includes html files or directories
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
		// project::center(50, "~~~ Options ~~~ ");
		std::cout << "\t1. Convert all files (but not sub-folders)." << endl;
		std::cout << "\t2. Convert all files and sub-folder files." << endl;
		std::cout << "\t3. Convert only some of the files shown." << endl;
		if (hasDirectory)
		{
			std::cout << "\t4. Look through one of the sub-folders listed." << endl;
		}

		// cout << "\t4. " << (hasDirectory ? "Look through one of the sub-folders listed." : "Go back to the previous folder.") << endl;
		std::cout << "\t> ";
	}
	else
	{
		cout << "Hey!! This thing's empty!!!!!!!!!!!!!!!!" << endl;
	}
}

void showMaze()
{
	cout << "The maze navigation looks like: " << endl;
	for (auto const &entry : fileMaze)
	{
		cout << entry << " / ";
	}
	cout << endl;
}

pair<bool, cssRule> getRule(string &str)
{
	// gets a css rule from the stylesheet vector based on the class name given
	cssRule t;
	bool m{false};
	// pair<bool, cssRule> p;
	for (auto r : stylesheet)
	{
		if (r.klass == str)
		{
			// cout << "located the class \"" << str << "\"." << endl;
			t = r;
			m = true;
			break;
		}
	}
	// p = make_pair(m, t);
	return make_pair(m, t);
}

pair<bool, cssRule> getRule(string &e, string &c, string &g)
{
	// same as just the class but this time we specify other things lol
	cssRule t(e, c, g);
	bool m{false};
	// pair<bool, cssRule> p;
	for (auto r : stylesheet)
	{
		if (r.klass == c)
		{
			// cout << "located the class \"" << str << "\"." << endl;
			t = r;
			m = true;
			break;
		}
	}
	// p = make_pair(m, t);
	return make_pair(m, t);
}

void snip()
{
	try
	{
		tmp.erase(tmp.begin(), tmp.begin() + trimAway);
		// if (listSwitch) {

		// 	// cout << *cow << endl;
		// }
		// cout << tmp << endl;
	}
	catch (out_of_range)
	{
		cout << "out of range!!" << endl;
		// break;
		tmp = "";
	}
}

void handleLi()
{
	cleanLine << full; // add the <li> to it
	snip();			   // then we have to snip it
	regex moo{"(.*?)(?=<)"};
	regex_iterator cow(tmp.begin(), tmp.end(), moo);
	for (int i{0}; i < (*cow).size(); i++)
	{
		cout << i << ": " << (*cow)[i].str() << endl;
	}
	innerHTML = (*cow)[0].str();
	cleanLine << innerHTML;
	tmp.erase(tmp.begin(), tmp.begin() + innerHTML.length());
}

string currentPath()
{
	string t{""};
	for (auto const &entry : fileMaze)
	{
		t += entry + " / ";
	}
	return t;
}

void makeDir()
{
	//
	string op{"output"};
	if (!filesystem::directory_entry(op).exists())
	{
		filesystem::create_directory(op); // make the output folder if dne
	}
	for (auto const &ent : fileMaze)
	{
		op += "/" + ent;
		if (!filesystem::directory_entry(op).exists())
		{
			// if a particular subfolder doesn't exist, then create it
			filesystem::create_directory(op + "/");
			cout << "Now creating directory: " << tmp << endl;
		}
	}
}

void open(filesystem::directory_entry p)
{
	makeDir(); // make the directories if necessary
	string op{"output/" + currentPath() + currFile.path().stem().string() + ".html"};
	// should only really be a thing for the cleaned, so this should be our final output path as a string
	// cleaned.open(p.path());
	cout << "Now opening: " << op << endl;
	cleaned.open(op);
	if (!cleaned.is_open())
	{
		cleaned.clear();
		cleaned.open(op, ios::out);
		cleaned.close();
		cleaned.open(op);
	}
	//
}