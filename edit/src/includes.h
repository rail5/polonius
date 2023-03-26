/* Needed for I/O */
#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif
/* Needed for getopt / program arguments */
#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
#endif
/* Needed for file_exists() function */
#ifndef SYS_STAT_H
	#define SYS_STAT_H
	#include <sys/stat.h>
#endif
/* Needed for explode() function */
#ifndef SSTREAM
	#define SSTREAM
	#include <sstream>
#endif
/* Needed for explode() function */
#ifndef VECTOR
	#define VECTOR	
	#include <vector>
#endif
/* Needed for class_file & isolate_path_from_filename() function */
#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif
/* Needed for access() in bool file_is_writable() */
#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif
/* Needed for POSIX file locks */
#ifndef SYS_FILE_H
	#define SYS_FILE_H
	#include <sys/file.h>
#endif
/* Needed for to_lower() function and is_number() function */
#ifndef ALGORITHM
	#define ALGORITHM
	#include <algorithm>
#endif
/* Needed for actual file edits */
#ifndef FSTREAM
	#define FSTREAM
	#include <fstream>
#endif

/* inline bool file_exists(std::string name) returns true if the file exists */
#include "../../shared_functions/file_exists.cpp"
/* inline std::string isolate_path_from_filename(std::string filename) returns the path to the directory a given file is in */
#include "../../shared_functions/isolate_path_from_filename.cpp"
/* inline bool file_is_writable(std::string filename) returns true if current UID has write permissions to that path */
#include "../../shared_functions/file_is_writable.cpp"
/* inline std::string to_lower(std::string &input) returns the input string in all-lowercase */
#include "../../shared_functions/to_lower.cpp"
/* std::vector<std::string> explode(std::string const &input, char delimiter, int maximum_number_of_elements = 0) returns a vector<string> split based on the given delimiter. Similar to PHP's explode() */
#include "../../shared_functions/explode.cpp"
/* inline bool is_number(const std::string &s) returns true if all chars in string are digits */
#include "../../shared_functions/is_number.cpp"
/* void remove_leading_whitespace(string &input) removes whitespace from the beginning of an input string */
#include "../../shared_functions/remove_leading_whitespace.cpp"

/* Contains the 'meat' of the program */
#include "namespace_editor.h"
#include "class_instruction.cpp"
#include "class_file.cpp"
