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
#include "../../shared/is_number.cpp"

/* file_exists(string name) returns true if the file exists on the system */
#include "../../shared/file_exists.cpp"

/* Contains the 'meat' of the program */
#include "namespace_reader.h"
#include "class_file.cpp"
