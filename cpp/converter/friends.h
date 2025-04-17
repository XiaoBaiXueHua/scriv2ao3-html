// this is really just a file to help manage all the goddamn functions i started making for main
#ifndef FWIENDSHIP
#define FWIENDSHIP

void showMaze();
void resetCurrentPath(filesystem::directory_entry);
string currentPath(); // returns the maze up to the current folder
void makeDir();
void makeDir(string);
void open(string &, filesystem::directory_entry, fstream &); // opens up the stream
string tupper(string);
bool tf(string);
pair<bool, cssRule> getRule(string &);
pair<bool, cssRule> getRule(const string);
pair<bool, cssRule> getRule(string &, string &, string &);

#endif