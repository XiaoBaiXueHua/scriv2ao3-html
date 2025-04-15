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

// configuration vars
bool configured{false}, prettify{true}, navigator{false}, recursive{false}, consolidate{false}, copysrc{false}, deletesrc{false}, batch{false};
char setf('\t');
unsigned int tabMultiplicity{1}; // how many chars should be printed at a time
string htmlFolder{"html"}, copyFolder{"converted"}, hr{"~***~"};

fstream raw, cleaned, copier; // input/output streams + a copier to keep a copy of the original exported html n subsequently delete the original from the html-to-process folder
stringstream sstr, sstr2;
filesystem::directory_entry currFile{htmlFolder}; // current file, initialized to the html folder
vector<filesystem::directory_entry> entries;	  // the vector to help us navigate i guess! and also the entries thing for displaying the stuff
vector<string> fileMaze{};
vector<cssRule> stylesheet;
string tmp{""}, tmp2{""}, tmp3{""};
string innerHTML{""};
string full{""}, guts{""};

stringstream cleanLine;
sanitize pls;

smatch smidge;
bool hasDirectory{false}, listSwitch{false}, tableSwitch{false};
int convertOpt{0}, trimAway{0}, tables{0};

void explorer();
void resetEntries();
void resetEntries(bool);
void showEntries();
void showMaze();
void cleaner();
pair<bool, cssRule> getRule(string &);
pair<bool, cssRule> getRule(const string);
pair<bool, cssRule> getRule(string &, string &, string &);
void snip();
void pushLine(vector<sanitize> &v);			// both pls && cleanLine are global, so this doesn't need to take those as args
void pushLine(vector<sanitize> &v, bool c); // conditional version
void pushLine(sanitize &s, vector<sanitize> &v, bool c);

string currentPath(); // returns the maze up to the current folder
void makeDir();
void open(filesystem::directory_entry); // opens up the cleaned thing

void resetCurrentPath(filesystem::directory_entry e)
{
	//
	if (e.path().has_parent_path())
	{
		fileMaze.clear(); // clear this out
		if (!regex_search(e.path().parent_path().string(), regex(string("^" + htmlFolder + "$"))))
		{
			tmp3 = e.path().parent_path().string().substr(htmlFolder.length() + 1); // parent path
			cout << tmp3 << " has more than just html as a parent. " << endl;
			// cout << "this entry's parent path is Only html/" << endl;
			int i{0};
			sstr2.clear();
			sstr2 << tmp3;
			cout << "new folder structure: " << endl;
			while (getline(sstr2, tmp2, '\\'))
			{

				cout << "\"" << tmp2 << "\"" << endl;
				if (tmp2 != htmlFolder)
				{

					fileMaze.push_back(tmp2); // remake this, n make sure NOT to include the html path
				}
			}
		}
	}
}
string tupper(string s)
{
	string u{""};
	for (const char c : s)
	{
		u += toupper(c);
	}
	return u;
}

bool tf(string s)
{
	if (tupper(s) == "TRUE" || s == "1")
	{
		return true;
	}
	else if (tupper(s) == "FALSE" || s == "0")
	{
		return false;
	}
	else
	{
		throw new logic_error("hey man. keywords true/false or 1/0 only.");
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
		configured = true;
		convertOpt = 3; // default is just one n you choose

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
				// count <<
				// now analyze the thing
				if (tupper(config[0]) == "SRC")
				{
					htmlFolder = config[1];
					currFile = filesystem::directory_entry(htmlFolder); // set this as well
				}
				else if (tupper(config[0]) == "PRETTIFY")
				{
					prettify = tf(config[1]);
				}
				else if (tupper(config[0]) == "SETFILL")
				{
					// no matter what, will only take the first char
					if (config[1] == "tab")
					{
						setf = '\t';
					}
					else if (config[1] == "space")
					{
						setf = ' ';
					}
					else
					{
						setf = char(config[1][0]); // otherwise, just take the first char of the string
					}
				}
				else if (tupper(config[0]) == "BATCH")
				{
					batch = tf(config[1]);
				}
				else if (tupper(config[0]) == "RECURSIVE")
				{
					recursive = tf(config[1]);
				}
				else if (tupper(config[0]) == "CONSOLIDATE")
				{
					consolidate = tf(config[1]);
				}
				else if (tupper(config[0]) == "DELETESRC")
				{
					deletesrc = tf(config[1]);
				}
				else if (tupper(config[0]) == "COPYSRC")
				{
					copysrc = tf(config[1]);
				}
				else if (tupper(config[0]) == "COPYFOLDER")
				{
					copyFolder = config[1];
				}

				sstr.clear(); // clear it at the end
				config.clear();
			}
		}

		// now that we're out,
		cout << "config complete." << endl;
	}
	else
	{
		cout << "configuration failed." << endl;
	}
	c.close(); // and then close the stream ofc
	c.clear();
}

int main()
{
	configure(); // configure first n foremost

	// pass along our static vars
	sanitize::prettify = prettify;
	sanitize::fill = setf;
	sanitize::hrStr = hr;

	cleaned << setfill(setf); // set this to tabs

	if (!configured || !navigator)
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
		resetCurrentPath(currFile); // since we're not doing sub-folders, this is enough
		for (auto const &dir_entry : filesystem::directory_iterator{currFile})
		{
			currFile = dir_entry;
			cleaner();
		}
		break;
	}
	case 2:
	{
		// all files AND sub-folders
		cout << "Cleaning all files And sub-folders." << endl;
		for (auto const &dir_entry : filesystem::recursive_directory_iterator{currFile})
		{

			resetCurrentPath(dir_entry); // this should be redone every time
			if (!dir_entry.is_directory())
			{
				//
				currFile = dir_entry;
				cleaner();
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
		// resetEntries();
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
			resetCurrentPath(currFile);
			for (auto const &dir_entry : filesystem::directory_iterator{currFile})
			{
				currFile = dir_entry;
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

	sanitize *sanPtr = new sanitize;

	// bool listSwitch{false}; // bool for list mode
	bool styleSwitch{false}, bodySwitch{false}, bqtSwitch{false};
	listSwitch = false;
	// vector<string> lines{}; // actually let's just have a vector of the body lines. i think that'll make it easier to work with in the end
	string parentEl{"ul"}; // parent element for list items
	int bodyLine{0}, tables{0};

	// vector<pair<string, cssRule>> linear;
	vector<sanitize> linear;
	float medianFontSize{1.00};
	// vector<string> cssRules{};
	// cleaned.open("output/test.html"); // just the test for now
	cleaned.clear();
	stylesheet.clear();

	// go through the whole thing once
	while (getline(raw, tmp))
	{
		bodyLine++;
		string untrimmed{tmp}; // untrimmed version for list items maybe
		tmp = trim(tmp);	   // trim owo
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

			// cout << "line: " << bodyLine << endl;
			cleanLine.clear();
			trimAway = 0; // integer to let us know the index of the proper substring

			bool prevClosing{false};
			regex anyEl("^(<(.*?)>)");

			while (tmp != "")
			{
				regex_iterator elemental(tmp.begin(), tmp.end(), anyEl); // first one is the full element, the other is its guts; we mostly work based off what the guts are
				full = (*elemental)[0].str();
				guts = (*elemental)[2].str();
				bool closing{guts[0] == '/'}; // boolean for if this is a closing element

				trimAway = full.length(); // default

				regex tagStructure("/?((\\w|\\d)+)(\\sclass=\"(.*?)\")?"); // this results in 0 = guts, 1 = element name, and 4 = class name

				regex_match(guts, smidge, tagStructure);
				// cout << "\tlooking at smidges: " << endl;
				// for (const auto s : smidge)
				// {
				// 	cout << "(" << s << ")\t";
				// }
				// cout << endl;
				string currentEl{smidge[1].str()};
				string currentClass{smidge[4].str()};
				// cout << "currentEl: " << currentEl << "\t\t" << "currentClass: " << currentClass << endl;
				pair<bool, cssRule> daRule = getRule(currentEl, currentClass, guts);
				cssRule relevant = daRule.second;
				bool endling{((currentEl == "p") && closing)}; // for now just closing paragraphs; will adjust as needed later
				// it also does not include "li" bc those get handled separately n will break on their own, so it would be redundant to include them in endling

				// the way tables will be handled is that we should probably have another sanitize as "plsChild" to hold onto the tds, and have the main "pls" hold onto the trs
				bool block{relevant.display != "inline"}, inList{((currentEl == "ol" || currentEl == "ul"))}, inRow{currentEl == "tr"};

				// reset the parent els and list switches n stuff Before we reset pls
				if (inList)
				{
					parentEl = currentEl;
				}
				listSwitch = ((inList && !closing) || (currentEl == "li")); // there. my god.

				if (currentEl == "table") // don't turn on the table switch bc that's more for handling tds
				{
					cout << "ooo we are in a tableeee" << endl;
					parentEl = currentEl;
					if (!closing)
					{
						tables++; // increment this
					}
				}

				// i think the way this n all the switches work might be causing issues w/the list turning off
				if (block) // so. technically this just means "not an inline"
				{
					// tbh. probably have an algorithm or w/e here determine what the fucking element should be lol
					// cout << "resetting pls..." << endl;
					// cout << "\t(listSwitch || tableSwitch): [" << (listSwitch || tableSwitch) << "] ? (parentEl) [" << parentEl << "] : (relevant.parent) [" << relevant.parent << "]" << endl;
					pls = sanitize("", cssRule(((listSwitch || tableSwitch) ? parentEl : relevant.parent), currentClass, (daRule.first ? relevant.guts : ""))); // make sure that the guts aren't just the same as the current el, to avoid accidentally doubling up on that
				}

				if (inList || currentEl == "table") // nothing left to do if we're in a lonely list or table opener
				{
					break;
				}

				// cout << "line " << bodyLine << "; listSwitch: " << listSwitch << ";\tcurrentEl: " << setw(8) << currentEl << ";\tpls.el:" << setw(8) << pls.el << "; relevant.parent: " << setw(8) << relevant.parent << endl;
				if (inRow)
				{
					// more determinations
					if (closing)
					{
						// if it's a closing <tr>, then probably just push pls to linear n break huh.
						pls.setIndex(tables); // set the index
						pushLine(linear);
					}
					break; // trs are always solo lines so don't bother processing the rest of this loop
				}

				if (currentEl == "li") // list item handling
				{

					if (!closing)
					{
						// can't be doing this if it's an orphaned closing tag

						snip();			 // cut off the <li>
						innerHTML = tmp; // now that we snipped off the opening tag, we can just do this

						if (regex_search(innerHTML, regex("</li>")))
						{
							// if there's an </li> at the end, cut it off too
							innerHTML = innerHTML.substr(0, innerHTML.length() - 5);
						}

						regex spanner("(<span\\sclass=\"((\\w|\\d|\\-)+)\">)([^<]+)?(</span>)"); // picks out spans whereing 0 is token, 1 is full span element, 2 is class name, 3 is last digit/letter of said class, 4 is inner html, and 5 is the closing tag
						// regex spanner("(<span\\sclass=\"((\\w|\\d|\\-)+)\">)"); // picks the first span only; full class name will be 2
						if (regex_search(innerHTML, spanner))
						{
							cout << "there are spans to take care of in this li." << endl;
							// cout << innerHTML << endl;
							while (regex_search(innerHTML, spanner))
							{

								regex_iterator cow(innerHTML.begin(), innerHTML.end(), spanner);
								for (int i{0}; i < (*cow).size(); i++) { 
									cout << i << ". " << (*cow)[i] << endl;
								}
								// const string currSpan{(*cow)[0].str()};
								const string currKlass{(*cow)[2].str()};
								daRule = getRule(currKlass); // we can reuse this. i don't see why not
								relevant = daRule.second;
								// string inner{(*cow)[4].str()};
								// if (daRule.first) {
								// 	inner = relevant.printTag() + inner + relevant.printClose();
								// }
								// innerHTML = regex_replace((*cow)[0], inner);

								// cout << "replacing [" << currSpan << "] with [" << (daRule.first ? relevant.printTag() : "") << "]." << endl;

								// // cout << "daRule.first ? " << daRule.first << " relevant.printTag() " << relevant.printTag() << endl;

								// innerHTML = regex_replace(innerHTML, regex(currSpan), daRule.first ? relevant.printTag() : ""); // whether they get removed or simply printed differently depends on whether the span Matters
								
								// cout << "and replacing [</span>] with [" << (daRule.first ? relevant.printClose() : "") << "]." << endl;
								// // cout << "and replacin"
								// innerHTML = regex_replace(innerHTML, regex("</span>"), daRule.first ? relevant.printClose() : "");
							}
						}

						regex spaces("^\\s+"); // get spaces from the start
						// regex_match(untrimmed, smidge, spaces);
						regex_token_iterator oi(untrimmed.begin(), untrimmed.end(), spaces);
						// cout << "this would be a setw(" << (*oi).str().length() << "); adding " << (floor((*oi).str().length() / 2) - 1) << endl;
						// cout << "pls currently has an indent of: " << pls.length() << endl;
						pls += (floor((*oi).str().length() / 2) - 1); // add the indent. also floor it just in case

						cleanLine << innerHTML;

						// we shouldn't really need the final snip since tmp gets reset anyway
						// trimAway = innerHTML.length();
						// cout << "trimming away " << trimAway << " chars from " << tmp << endl;
						// snip();
					}
					else
					{
						// cout << "currentEl: " << currentEl << "\tinList: " << inList << endl;
						// cout << "untrimmed: " << untrimmed << endl;
						// listSwitch = false;
						// cout << "break due to orphaned li." << endl;
						pls.reset(); // reset and do Not push it
						break;
					}
					// cout << "break due to unclosed list item." << endl;
					pushLine(linear); // have to push this or else it will go missing
					break;
				}
				else if (currentEl == "td") // table handling
				{
					if (!closing)
					{
						// if it's not closing, then we turn some switch on or off or w/e
						tableSwitch = true;
					}
					else
					{
						// otherwise, closing td gets added to the pls, closing tr gets pushed to linear, n we do some switching stuff
						tableSwitch = false;
						(*sanPtr) += cleanLine; //

						pls += *sanPtr; // add the sanPtr to pls
						(*sanPtr).reset();
					}

					(*sanPtr).el = currentEl;

					break; // and then end it bc tds are on solo lines
				}
				else if (currentEl != "")
				{
					// cout << "\n\nCURRENT EL: " << currentEl << endl;
					if (closing)
					{
						if (block)
						{
							cout << "now closing a " << currentEl << " element." << endl;
						}
						if (!endling || tableSwitch) // if we're in a table, then you should be adding the end tag to it anyway
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
							else
							{
								cout << "wadda hell thing are we trying to close: " << currentEl << endl;
							}
						}
						else
						{
							cleanLine << full; // if it's classless, then just write the whole thing over
						}
					}
					snip();
				}
				else // this happens for <br />, <img>, <a>... the sort of things that have attributes w/o classes
				{
					if (!closing) // shouldn't have to deal with this anymore, but we'll see still
					{
						if (tmp.length() <= 1)
						{
							tmp = "";
						}
						else if (!listSwitch) // later when handling links, here's a bit for  helping w/degoogling links: &#38;sa=D&#38;source=editors&#38;ust=\d+&#38;usg=[^"]+
						{
							if (guts[0] != 'a') // check for non-link elements
							{
								cout << "\ncurrentEl: " << currentEl << " guts: " << guts << " full: " << full << endl;
							}
							cleanLine << full; // we'll fix this up more later when working with those wretched list items
						}
						else
						{
							cout << "...but smth weird happened.\n\t" << tmp << endl;
							cout << "\ncurrentEl: " << currentEl << " guts: " << guts << " full: " << full << endl;
							// cout << "break due to list switch stuffs." << endl;
							break; // break it if we're in list mode
						}

						snip();
					}
				}

				if (closing)
				{
					pushLine((tableSwitch ? (*sanPtr) : pls), linear, endling && !tableSwitch); // add it to the pointer if tableSwitch is on

					if (endling && !tableSwitch) // adding in the Not tableSwitch means that it also gets to finish putting stuff into the td
					{
						// cout << "break due to endling." << endl;
						break;
					}
				}
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
		// cout << "\t\t****** LINE " << i << " ******" << endl;
		// cout << "debug prev: ";
		// prev.debug(false);
		// cout << "debug current: ";
		// current.debug(false);
		// cout << "debug next: ";
		// next.debug(false);

		// open the table
		if ((current.tableIndex != prev.tableIndex) && (current.display == "table")) // making sure that our current one has parentage ensures we don't print an extra, like, <p> tag when switching from a table to smth non-tabular
		{
			cleaned << current.printParent(); // print the <table>
			if (sanitize::prettify)
			{
				cleaned << endl;
			}
		}

		// open the blockquote
		if (current.bqtMode)
		{
			// so if we're currently in blockquote mode
			if (!prev.bqtMode)
			{
				// default for bqtMode is false, so we don't need to worry abt doing some shit wrong
				cleaned << current.printParent();
				if (sanitize::prettify)
				{
					cleaned << endl;
				}
			}
		}
		// open up the list
		if (current.listMode)
		{
			long unsigned int first{prev.length()}, second{current.length()};
			if (!prev.listMode || first < second)
			{

				if (!prev.listMode) // if the previous one wasn't a list, then we gotta start from scratch regardless of how many indents prev had
				{
					first = 0;
				}
				while (first < second)
				{
					if (sanitize::prettify)
					{
						cleaned << setw(first) << "";
					}
					cleaned << current.printParent();
					if (sanitize::prettify)
					{
						cleaned << endl;
					}
					first++;
				}
			}
		}
		// tab our shit if prettify is on
		if (current.length() > 0 && sanitize::prettify)
		{
			// now we print our tabs
			cleaned << setw(current.length()) << "";
		}

		// print the element
		cleaned << current;
		// end the line if prettify is on
		if (sanitize::prettify)
		{
			cleaned << endl;
		}

		// close the list
		if (current.listMode)
		{
			long unsigned int first{next.length()}, second{current.length()};
			if (!next.listMode || first < second)
			{
				if (!next.listMode) // if the next one isn't a list, then we must cut off All the lists. this will probably have to get more elaborate when nesting ul and ol w/in each other but whatever not my problem Right Now
				{
					first = 0; // set this one to 0
				}
				while (first < second)
				{
					// so while the next one is less than the current one
					if (sanitize::prettify)
					{
						cleaned << setw(second - 1) << "";
					}
					cleaned << current.closeParent();
					if (sanitize::prettify)
					{
						cleaned << endl;
					}
					second--; // going backwards here
				}
			}
		}

		// close the blockquote
		if (current.bqtMode)
		{
			if (!next.bqtMode)
			{
				cleaned << current.closeParent();
				if (sanitize::prettify)
				{
					cleaned << endl;
				}
			}
		}

		// close the table
		if ((next.tableIndex != current.tableIndex) && (current.display == "table"))
		{
			cleaned << current.closeParent(); // print the </table>
			if (sanitize::prettify)
			{
				cleaned << endl;
			}
		}
	}

	raw.close(); // close the streaaaaams
	cleaned.close();

	sanPtr = nullptr;
	delete sanPtr;
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
	entries.clear();	  // start by clearing out the vector for display things
	hasDirectory = false; // reset this
	int numFiles{0};
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
		if (hasDirectory)
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

pair<bool, cssRule> getRule(const string str)
{
	//
	cssRule t;
	bool m{false};
	// pair<bool, cssRule> p;
	for (auto r : stylesheet)
	{
		if (r.klass == str)
		{
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
	for (auto r : stylesheet)
	{
		if (r.klass == c)
		{
			t = r;
			m = true;
			break;
		}
	}
	return make_pair(m, t);
}

void snip()
{
	try
	{
		tmp.erase(tmp.begin(), tmp.begin() + trimAway);
	}
	catch (out_of_range)
	{
		cout << "out of range!!" << endl;
		tmp = "";
	}
}

void pushLine(vector<sanitize> &v)
{
	pls += cleanLine; // add the inner HTML to the pls

	// clear the line Before we break the thing
	stringstream whee("");
	cleanLine.swap(whee); // can we just. clear it like that.
	v.push_back(pls);
	pls.reset();
}

void pushLine(vector<sanitize> &v, bool c)
{
	pushLine(pls, v, c); // ...it really just is just "pls by default" here lol
}

void pushLine(sanitize &s, vector<sanitize> &v, bool c)
{
	s += cleanLine;
	// cout << "s: " << s << endl; // add innerHTML to the sanitize
	stringstream whee("");
	cleanLine.swap(whee); // can we just. clear it like that.
	if (c)				  // if tmp is empty or we're closing a paragraph
	{
		v.push_back(s);
		s.reset();
	}
}

string currentPath()
{
	string t{""};
	for (auto const &entry : fileMaze)
	{
		t += entry + "/";
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