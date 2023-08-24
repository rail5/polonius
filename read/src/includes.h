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
/* Needed for process_bytecodes() function */
#ifndef REGEX
	#define REGEX
	#include <regex>
#endif

/* Shared definitions */
#include "../../shared/definitions.h"

/* inline bool is_number(const std::string &s) returns true if all chars in string are digits */
#ifndef FN_IS_NUMBER
	#define FN_IS_NUMBER
	#include "../../shared/is_number.cpp"
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
