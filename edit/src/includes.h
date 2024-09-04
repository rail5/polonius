/***
 * Copyright (C) 2024 rail5
*/

#ifndef EDIT_SRC_INCLUDES_H_
#define EDIT_SRC_INCLUDES_H_

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
/* Needed for explode() and process_bytecodes() functions */
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
/* Needed for access() in bool file_is_writable() & in file_exists() */
#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif
/* Needed for POSIX file locks */
#ifndef SYS_FILE_H
	#define SYS_FILE_H
	#include <sys/file.h>
#endif
/* Needed for some file checks (namely is_directory() function) */
#ifndef SYS_STAT
	#define SYS_STAT
	#include <sys/stat.h>
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
/* Needed for process_bytecodes() function */
#ifndef REGEX
	#define REGEX
	#include <boost/regex.hpp>
#endif


/* inline bool file_exists(std::string name) returns true if the file exists */
#ifndef FN_FILE_EXISTS
	#define FN_FILE_EXISTS
	#include "../../shared/file_exists.cpp"
#endif

/* inline std::string isolate_path_from_filename(std::string filename) returns the path to the directory a given file is in */
#ifndef FN_ISOLATE_PATH_FROM_FILENAME
	#define FN_ISOLATE_PATH_FROM_FILENAME
	#include "../../shared/isolate_path_from_filename.cpp"
#endif

/* inline bool file_is_writable(std::string filename) returns true if current UID has write permissions to that path */
#ifndef FN_IS_WRITABLE
	#define FN_IS_WRITABLE
	#include "../../shared/file_is_writable.cpp"
#endif

/* inline bool is_directory(std::string path) return true if the given path points to a directory */
#ifndef FN_IS_DIRECTORY
	#define FN_IS_DIRECTORY
	#include "../../shared/is_directory.cpp"
#endif

/* inline std::string to_lower(std::string &input) returns the input string in all-lowercase */
#ifndef FN_TO_LOWER
	#define FN_TO_LOWER
	#include "../../shared/to_lower.cpp"
#endif

/* std::vector<std::string> explode(std::string const &input, char delimiter, int maximum_number_of_elements = 0) returns a vector<string> split based on the given delimiter. Similar to PHP's explode() */
#ifndef FN_EXPLODE
	#define FN_EXPLODE
	#include "../../shared/explode.cpp"
#endif

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

/* void remove_leading_whitespace(string &input) removes whitespace from the beginning of an input string */
#ifndef FN_REMOVE_LEADING_WHITESPACE
	#define FN_REMOVE_LEADING_WHITESPACE
	#include "../../shared/remove_leading_whitespace.cpp"
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

/* Shared definitions */
#include "../../shared/definitions.h"

/* Contains the 'meat' of the program */
#include "namespace_editor.h"
#include "class_instruction.cpp"
#include "class_file.cpp"

#endif // EDIT_SRC_INCLUDES_H_
