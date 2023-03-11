#include "includes.h"

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
#endif

using namespace std;

int main(int argc, char* argv[]) {

	string program_name = "polonius-reader";
	
	string program_version = "0.1";
	
	string program_author = "rail5";

	string helpstring = program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author + "\n\nThis is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\nUsage: " + program_name + " -i filename\n\nOptions:\n  -i\n  --input\n    Specify input file to read\n\n  -s\n  --start\n    Specify byte number to start reading from\n\n  -l\n  --length\n    Specify how many bytes to read\n\n  -h\n  --help\n    Display this message\n\nExample:\n  " + program_name + " --input ./file.txt --start 50 --length 10\n";
	
	/*
	Necessary info for the program to do its job
	The file to read (-i),
	The byte to start from (-s) [0 if unspecified]
	The number of bytes to read (-l) [-1 if unspecified]
		"amount_to_read" == -1 will result in reading from the start position to the end of the file
	*/
	string file_to_read = "";
	int start_position = 0;
	int amount_to_read = -1;
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] =
	{
		{"input", required_argument, 0, 'i'},
		{"start", required_argument, 0, 's'},
		{"length", required_argument, 0, 'l'},
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:l:h", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				file_to_read = optarg;
				break;
			case 's':
				if (!is_number(optarg)) {
					cerr << program_name << ": '" << optarg << "' is not an integer" << endl << "Use -h for help" << endl;
					return 1;
				}
				start_position = (int)atol(optarg);
				break;
			case 'l':
				if (!is_number(optarg)) {
					cerr << program_name << ": '" << optarg << "' is not an integer" << endl << "Use -h for help" << endl;
					return 1;
				}
				amount_to_read = (int)atol(optarg);
				break;
			case 'h':
				cout << helpstring;
				return 0;
				break;
			case '?':
				if (optopt == 'i' || optopt == 's' || optopt == 'l') {
					cerr << program_name << ": Option -" << (char)optopt << " requires an argument" << endl << "Use -h for help" << endl;
					return 1;
				}
				break;
		}
	}
	
	// Make sure we got an input file
	if (file_to_read == "") {
		cerr << helpstring;
		return 1;
	}
	
	reader::file the_file;
	
	if (!the_file.init(file_to_read)) {
		cerr << program_name << ": " << the_file.get_init_error_message() << endl;
		return 1;
	}
	
	// If -l / --length wasn't set, just set it to the full length of the file
	if (amount_to_read == -1) {
		amount_to_read = the_file.get_file_length();
	}
	
	string buffer = the_file.read(start_position, amount_to_read);
	
	cout << buffer;

	return 0;
}
