/* Needed for I/O */
#include <iostream>
/* Needed for getopt */
#include <getopt.h>
/* Needed for "is_number()" function (std::find_if) */
#include <algorithm>
/* Needed for binary file reads */
#include <fstream>
/* Needed for "file_exists()" function */
#include <sys/stat.h>

/* is_number(const std::string& s) returns true if all chars in string are digits */
#include "../../shared_functions/is_number.cpp"

/* file_exists(string name) returns true if the file exists on the system */
#include "../../shared_functions/file_exists.cpp"

/* Contains the 'meat' of the program */
#include "namespace_reader.h"
#include "class_file.cpp"
