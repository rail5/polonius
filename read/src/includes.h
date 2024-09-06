/***
 * Copyright (C) 2024 rail5
*/

#ifndef READ_SRC_INCLUDES_H_
#define READ_SRC_INCLUDES_H_

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
/* Needed for "is_number()" function (std::find_if) */
#ifndef ALGORITHM
	#define ALGORITHM
	#include <algorithm>
#endif
/* Needed for binary file reads */
#ifndef FSTREAM
	#define FSTREAM
	#include <fstream>
#endif
/* Needed for "file_exists()" function */
#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif
/* Needed for some file checks (namely is_directory() function) */
#ifndef SYS_STAT
	#define SYS_STAT
	#include <sys/stat.h>
#endif
/* Needed for process_bytecodes() function and for regex searches */
#ifndef REGEX
	#define REGEX
	#include <boost/regex.hpp>
#endif

/* Shared definitions */
#include "../../shared/definitions.h"

/* inline bool is_number(const std::string &s) returns true if all chars in string are digits */
#ifndef FN_IS_NUMBER
	#define FN_IS_NUMBER
	#include "../../shared/is_number.cpp"
#endif

/* std::vector<std::string> explode(std::string const &input, char delimiter, bool can_escape, int maximum_number_of_elements = 0) returns a vector<string> split based on the given delimiter. Similar to PHP's explode() */
#ifndef FN_EXPLODE
	#define FN_EXPLODE
	#include "../../shared/explode.cpp"
#endif

/* inline bool string_starts_with(std::string const &haystack, std::string const &needle) returns true if 'haystack' starts with substring 'needle' */
#ifndef FN_STRING_STARTS_WITH
	#define FN_STRING_STARTS_WITH
	#include "../../shared/string_starts_with.cpp"
#endif

/* inline bool string_ends_with(std::string const &haystack, std::string const &needle) returns true if 'haystack' ends with substring 'needle' */
#ifndef FN_STRING_ENDS_WITH
	#define FN_STRING_ENDS_WITH
	#include "../../shared/string_ends_with.cpp"
#endif

/* inline int parse_block_units(const std::string &user_input) converts sizes such as '10K' or '16M' to equivalent in bytes */
#ifndef FN_PARSE_BLOCK_UNITS
	#define FN_PARSE_BLOCK_UNITS
	#include "../../shared/parse_block_units.cpp"
#endif

/* inline bool file_exists(std::string name) returns true if the file exists */
#ifndef FN_FILE_EXISTS
	#define FN_FILE_EXISTS
	#include "../../shared/file_exists.cpp"
#endif

/* inline bool is_directory(std::string path) return true if the given path points to a directory */
#ifndef FN_IS_DIRECTORY
	#define FN_IS_DIRECTORY
	#include "../../shared/is_directory.cpp"
#endif

/* string process_escapedchars(string input) processes strings containing '\n', '\t', and '\\' (literal), replacing them with the actual characters they represent */
#ifndef FN_PROCESS_ESCAPEDCHARS
	#define FN_PROCESS_ESCAPEDCHARS
	#include "../../shared/process_escapedchars.cpp"
#endif

/* string process_bytecodes(string input) processes strings containing escaped byte-codes (such as \x00 or \xFF), replacing all byte-codes with the actual bytes they represent */
#ifndef FN_PROCESS_BYTECODES
	#define FN_PROCESS_BYTECODES
	#include "../../shared/process_bytecodes.cpp"
#endif

/* vector<string> parse_regex(string expression) splits a regular expression into its component parts */
/* e.g.: parse_regex("[a-z]+123") returns { "[a-z]+", "1", "2", "3" } */
/* vector<string> create_sub_expressions(string expression) [same file] runs parse_regex() and then recombines the parts into smaller expressions */
/* e.g.: create_sub_expressions("[a-z]+123") returns { "[a-z]+12", "[a-z]+1", "[a-z]+" } */
#ifndef FN_PARSE_REGEX
	#define FN_PARSE_REGEX
	#include "../../shared/parse_regex.cpp"
#endif

/* Contains the 'meat' of the program */
#include "namespace_reader.h"
#include "class_file.cpp"

#endif // READ_SRC_INCLUDES_H_
