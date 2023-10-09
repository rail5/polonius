/* Needed for I/O */
#ifndef STDIOH
	#define STDIOH
	#include <stdio.h>
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

#ifndef SSTREAM
	#define SSTREAM
	#include <sstream>
#endif

#ifndef VECTOR
	#define VECTOR	
	#include <vector>
#endif


int main() {
	std::string test;
	test = "yo";
	return 0;
}
