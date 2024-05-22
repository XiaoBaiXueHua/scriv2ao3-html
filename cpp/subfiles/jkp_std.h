// since this file is just to facilitate my own laziness, i'm going to code it like it's not getting graded
#ifndef jkp_std
#define jkp_std
#include <iostream>
#include <iomanip>
#include <string>
#include <vector>
#include <fstream>
class project {
	public:
		project(){}; //default constructor

		//use of "static" lets me not have to create a new project instance; instead just doing project::title("str")
		static void title(std::string project_name) { //if i were doing this for others to use i'd also add in a var for user name, but this is really just bc i'm lazy and don't want to have to format my titles all the time. 
		std::string stars{"****************************************************************"}; //64 max should be enough, right?
		std::string titleLine{"* " + project_name + " *"};
		std::string gap{"*" + spacer(titleLine.length()-2, ' ') + "*"};
		std::string border{stars.substr(0, titleLine.length())};
		unsigned int w{50};
		std::vector<std::string>titleBloc = {border, gap, titleLine, gap, border};
		centerBloc(w, titleBloc);
		std::cout << "\n\n";
		}

		static void center(unsigned int width, std::string str) {
			unsigned int str_len{str.length()};
			double w{((width < str_len) ? (width + str_len) : (width + ceil(str_len/2)))}; // "ceil" is a double, so have to use double here as well
			std::cout << std::setw(w) << str << std::endl;
		}
		static void centerBloc(int width, std::vector<std::string>&strs) { 
			for (auto str : strs) {
				center(width, str);
			}
		}

		static std::string spacer(int i, char space) { //can't seem to find a function that lets you utilize setw() in a string
			std::string str{""};
			for (int j{0}; j < i; j++) {
				str += space;
			}
			return str;
		}

		static void logger(std::string str) {
			//std::fstream log;
			//log.open("subfiles/log.txt");
			if (!log.is_open()) { //if it's not already open
				try {
					log.open("subfiles/log.txt"); //first check to see if i just forgot to open it
					logger(str); //then log as usual
				} catch(std::error_code) {
					log.clear(); //otherwise, clear the stream
					log.open("subfiles/log.txt", std::ios::out); //make the log file
					log.close(); //close the file
					log.open("subfiles/log.txt"); //to open it again cleanly
				}
				
			};
			log << str;
		}
	private:
	static std::fstream log;
};
#endif //end jkp_std