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
#ifndef SYS_STAT_H
	#define SYS_STAT_H
	#include <sys/stat.h>
#endif

/* inline bool is_number(const std::string &s) returns true if all chars in string are digits */
#include "../../shared_functions/is_number.cpp"

/* file_exists(string name) returns true if the file exists on the system */
#include "../../shared_functions/file_exists.cpp"

/* Contains the 'meat' of the program */
#include "namespace_reader.h"
#include "class_file.cpp"
