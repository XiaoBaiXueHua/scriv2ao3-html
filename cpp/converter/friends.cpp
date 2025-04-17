#include "friends.h"
#include "options.h"

void showMaze()
{
	cout << "The maze navigation looks like: " << endl;
	for (auto const &entry : options::fileMaze)
	{
		cout << entry << " / ";
	}
	cout << endl;
}

void resetCurrentPath(filesystem::directory_entry e)
{
	string tmp{""};
	stringstream sstr2("");
	if (e.path().has_parent_path())
	{
		options::fileMaze.clear(); // clear this out
		if (!regex_search(e.path().parent_path().string(), regex(string("^" + options::htmlFolder + "$"))))
		{
			int i{0};
			sstr2.clear();
			sstr2 << e.path().parent_path().string().substr(options::htmlFolder.length() + 1);
			while (getline(sstr2, tmp, '\\'))
			{

				// cout << "\"" << tmp << "\"" << endl;
				if (tmp != options::htmlFolder)
				{

					options::fileMaze.push_back(tmp); // remake this, n make sure NOT to include the html path
				}
			}
		}
	}
}
string currentPath()
{
	string t{""};
	for (auto const &entry : options::fileMaze)
	{
		t += entry + "/";
	}
	return t;
}

void makeDir(string s)
{
	// string op{outputFolder};
	if (!filesystem::directory_entry(s).exists())
	{
		filesystem::create_directory(s); // make the output folder if dne
	}
	for (auto const &ent : options::fileMaze)
	{
		s += "/" + ent;
		if (!filesystem::directory_entry(s).exists())
		{
			// if a particular subfolder doesn't exist, then create it
			filesystem::create_directory(s + "/");
		}
	}
}

void makeDir()
{
	makeDir(options::outputFolder);
}

void open(string &folder, filesystem::directory_entry p, fstream &fstr)
{
	if (fstr.is_open())
	{
		// if it's already open, like if i forgot to close it, then close it again. ehe
		fstr.close();
	}

	makeDir(); // make the directories if necessary
	if (options::copysrc)
	{
		makeDir(options::copyFolder);
	}

	string op{folder + "/" + currentPath() + ((options::consolidate && folder != options::copyFolder) ? "index" : p.path().stem().string()) + ".html"};
	// should only really be a thing for the cleaned, so this should be our final output path as a string
	// cleaned.open(p.path());
	cout << "Now opening: " << op << endl;
	options::consolidate ? fstr.open(op, ios::app) : fstr.open(op);
	if (!fstr.is_open())
	{
		fstr.clear();
		fstr.open(op, ios::out);
		fstr.close();
		options::consolidate ? fstr.open(op, ios::app) : fstr.open(op);
		// cleaned.open(op);
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

pair<bool, cssRule> getRule(string &e, string &c, string &g)
{
	// same as just the class but this time we specify other things lol
	cssRule t(e, c, g);
	bool m{false};
	for (auto r : cssRule::stylesheet)
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

pair<bool, cssRule> getRule(const string str)
{
	//
	cssRule t;
	bool m{false};
	// pair<bool, cssRule> p;
	for (auto r : cssRule::stylesheet)
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

pair<bool, cssRule> getRule(string &str)
{
	// gets a css rule from the stylesheet vector based on the class name given
	cssRule t;
	bool m{false};
	// pair<bool, cssRule> p;
	for (auto r : cssRule::stylesheet)
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
