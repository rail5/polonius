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

/* Contains the 'meat' of the program */
#include "namespace_reader.h"
#include "class_file.cpp"
