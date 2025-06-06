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
// #include "whatever.cpp"
#include "li-ruby.h"
// #include "li-ruby.cpp"
#include "friends.h"
// #include "friends.cpp"
#include "cleaner.h"

using namespace std;

cleaner::cleaner(filesystem::directory_entry e)
{
	currFile = e;

	cout << "Now cleaning: " << currFile.path().stem() << endl
		 << endl;
	resetCurrentPath(currFile); // only do it once per file opened

	raw.open(currFile.path()); // open the raw
	if (options::copysrc)
	{
		// open the copy stream
		open(options::copyFolder, currFile, copier); // this open comes from the friends file btw
	}
	open(options::outputFolder, currFile, cleaned); // open the output

	whoosh(); // do all that
}
cleaner::~cleaner()
{

	cout << "finished cleaning " << currFile.path().stem() << ".\n"
		 << endl;
	cout << "-------------------------" << "\n\n";
	raw.close();
	raw.clear();
	cleaned.close();
	cleaned.clear();
	if (options::copysrc)
	{
		copier.close();
		copier.clear();
	}

	sanPtr = nullptr;
	delete sanPtr;
	if (options::deletesrc)
	{
		cout << "Deleting " << currFile << "... bye bye!!!!" << endl;
		filesystem::remove(currFile); // delete the file if that's true
	}
}

void cleaner::snipe()
{
	snip(tmp, trimAway);
}
bool cleaner::incl(string k)
{
	return includes(k, relevant);
}

void cleaner::whoosh()
{
	cleaned << setfill(sanitize::fill);
	bool styleSwitch{false}, bodySwitch{false}, bqtSwitch{false};
	listSwitch = false;
	string parentEl{"ul"}; // parent element for list items
	int bodyLine{0}, tables{0};

	vector<sanitize> linear;
	float medianFontSize{1.00};
	cleaned.clear();
	cssRule::stylesheet.clear();

	// go through the whole thing once
	while (getline(raw, tmp))
	{
		if (options::copysrc)
		{
			copier << tmp << endl; // make da copy
		}
		if (raw.eof())
		{
			break;
		} // break at eof

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
				vector<cssRule> tmpStyles = cssRule::stylesheet;
				stable_sort(tmpStyles.begin(), tmpStyles.end(), [](const cssRule &a, const cssRule &b)
							{
								return a.fontSize < b.fontSize; // sorts from least to greatest
							});
				// if the median font size is greater than 0, then set it to whatever that is
				if (tmpStyles[floor((tmpStyles.size() - 1) / 2)].fontSize > 0)
				{
					medianFontSize = tmpStyles[floor((tmpStyles.size() - 1) / 2)].fontSize;
				}
				cssRule::stylesheet.clear();
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
					if (r.el != "span")
					{
						cssRule::stylesheet.push_back(r);
					}
				}
			}
			else
			{
				cssRule ayo(tmp);
				// cout << "el: " << ayo.el << endl;
				if (ayo.worthwhile)
				{ // ignore the body type elements
					cssRule::stylesheet.push_back(ayo);
				}
			}
		}
		if (bodySwitch)
		{

			if (tmp == "</body>")
			{
				bodySwitch = false;
				//<< endl;
				// break;
			}

			// cout << "line: " << bodyLine << endl;
			cleanLine.clear();
			trimAway = 0; // integer to let us know the index of the proper substring

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
				string currentEl{smidge[1].str()};
				string currentClass{smidge[4].str()};
				// cout << "currentEl: " << currentEl << "\t\tcurrentClass: " << currentClass << "\t\tguts: " << guts << endl;
				relevant = cssRule(currentEl, currentClass, ""); // reset this

				bool important{incl(currentClass)};			   // and then the "includes" function will automatically
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

				if (currentEl == "table" && !closing) // don't turn on the table switch bc that's more for handling tds
				{
					cout << "ooo we are in a tableeee" << endl;
					parentEl = currentEl;
					tables++; // increment this
				}

				// i think the way this n all the switches work might be causing issues w/the list turning off
				if (block) // so. technically this just means "not an inline"
				{
					// tbh. probably have an algorithm or w/e here determine what the fucking element should be lol
					// cout << "resetting pls..." << endl;
					// cout << "\t(listSwitch || tableSwitch): [" << (listSwitch || tableSwitch) << "] ? (parentEl) [" << parentEl << "] : (relevant.parent) [" << relevant.parent << "]" << endl;
					pls = sanitize("", cssRule(((listSwitch || tableSwitch) ? parentEl : relevant.parent), currentClass, (important ? relevant.guts : ""))); // make sure that the guts aren't just the same as the current el, to avoid accidentally doubling up on that
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

						snipe(); // cut off the <li>

						if (regex_search(tmp, regex("</li>")))
						{
							// if there's an </li> at the end, cut it off too
							tmp = tmp.substr(0, tmp.length() - 5);
						}

						regex spaces("^\\s+"); // get spaces from the start
						// regex_match(untrimmed, smidge, spaces);
						regex_token_iterator oi(untrimmed.begin(), untrimmed.end(), spaces);
						pls += (floor((*oi).str().length() / 2) - 1); // add the indent. also floor it just in case

						cleanLine << li(tmp); // add the clean line which has been wonderfully and fearfully cleaned by our new li class

						pushLine(linear); // have to push this or else it will go missing
					}
					else
					{
						// cout << "currentEl: " << currentEl << "\tinList: " << inList << endl;
						// cout << "untrimmed: " << untrimmed << endl;
						// listSwitch = false;
						// cout << "break due to orphaned li." << endl;
						pls.reset(); // reset and do Not push it
					}
					break; // break it no matter what
				}
				else if (currentEl == "td") // table handling
				{
					// i don't know why this works but doing "tableSwitch = !closing" doesn't but i guess we just have to live like this now
					if (!closing)
					{
						// if it's not closing, then we turn some switch on or off or w/e
						tableSwitch = true;
					}
					else
					{
						// otherwise, closing td gets added to the pls, closing tr gets pushed to linear, n we do some switching stuff
						tableSwitch = false;

						(*sanPtr) += cleanLine; // closing td gets added to the pls, closing tr gets pushed to linear, n we do some switching stuff

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
							if (important && !block) // if it's an element w/particular styling rules
							{
								cleanLine << relevant.printTag(); // only print this if the previous element was the same as the current element...?
							}

							// cleanLine << relevant.printTag(); // with the new modifications made to the class, we shouldn't have to check for anything now

							if (currentEl == "span")
							{
								// adjust the trim away bit
								trimAway = full.length() + innerHTML.length() + string("</" + currentEl + ">").length();

								cleanLine << innerHTML;
								if (important) // if we have a different replacement span
								{
									cleanLine << relevant.printClose();
								}
							}
						}
						else
						{
							cleanLine << full; // if it's classless, then just write the whole thing over
						}
					}
					snipe();
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

						snipe();
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

	if (options::consolidate)
		cleaned << "<hr />" << endl; // before closing the streams, add an hr btwn docs if they're to be consolidated
}

// pushLines
void cleaner::pushLine(vector<sanitize> &v)
{
	pushLine(pls, v, true);
}

void cleaner::pushLine(vector<sanitize> &v, bool c)
{
	pushLine(pls, v, c); // ...it really just is just "pls by default" here lol
}

void cleaner::pushLine(sanitize &s, vector<sanitize> &v, bool c)
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
