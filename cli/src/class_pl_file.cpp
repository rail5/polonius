#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

#ifndef SSTREAM
	#define SSTREAM
	#include <sstream>
#endif

using namespace std;

bool polonius::pl_file::init(string file) {
	file_path = file;
	
	initialized = true;
	return initialized;
}

string polonius::pl_file::read(int64_t start_position, int64_t end_position) {
	stringstream command_stream;
	stringstream result_stream;
	
	command_stream << "polonius-reader -i \"" << file_path << "\" -s " << start_position << " -l " << end_position;
	
	result_stream << polonius::command::exec(command_stream.str());
	
	return result_stream.str();
}

polonius::pl_file open_file(string file) {
	polonius::pl_file new_file;
	
	new_file.init(file);
	
	return new_file;
}
