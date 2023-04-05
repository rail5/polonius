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
	
	string program_version = "0.3";
	
	string program_author = "rail5";

	string helpstring = program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author + "\n\nThis is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\nUsage: " + program_name + " -i filename\n\nOptions:\n  -i\n  --input\n    Specify input file to read\n\n  -s\n  --start\n    Specify byte number to start reading from\n\n  -l\n  --length\n    Specify how many bytes to read\n\n  -V\n  --version\n    Print version number\n\n  -h\n  --help\n    Display this message\n\nExamples:\n  " + program_name + " --input ./file.txt --start 50 --length 10\n\n  " + program_name + " -s 50 -l 10 ./file.txt\n";
	
	/*
	Necessary info for the program to do its job
	The file to read (-i),
	The byte to start from (-s) [0 if unspecified]
	The number of bytes to read (-l) [-1 if unspecified]
		"amount_to_read" == -1 will result in reading from the start position to the end of the file
	*/
	string file_to_read = "";
	bool received_filename = false;
	int64_t start_position = 0;
	int64_t amount_to_read = -1;
	
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
		{"version", no_argument, 0, 'V'},
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:l:Vh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				if (received_filename) {
					cerr << "polonius-reader: Error: Multiple files specified" << endl;
					return EXIT_BADFILE;
				}
				file_to_read = optarg;
				received_filename = true;
				break;
				
			case 's':
				if (!is_number(optarg)) {
					cerr << program_name << ": '" << optarg << "' is not an integer" << endl << "Use -h for help" << endl;
					return EXIT_BADARG;
				}
				start_position = (int64_t)stoll(optarg);
				break;
				
			case 'l':
				if (!is_number(optarg)) {
					cerr << program_name << ": '" << optarg << "' is not an integer" << endl << "Use -h for help" << endl;
					return EXIT_BADARG;
				}
				amount_to_read = (int64_t)stoll(optarg);
				break;
				
			case 'V':
				cout << program_version << endl;
				return EXIT_SUCCESS;
				break;
				
			case 'h':
				cout << helpstring;
				return EXIT_SUCCESS;
				break;
				
			case '?':
				if (optopt == 'i' || optopt == 's' || optopt == 'l') {
					cerr << program_name << ": Option -" << (char)optopt << " requires an argument" << endl << "Use -h for help" << endl;
					return EXIT_BADOPT;
				}
				break;
		}
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		if (received_filename) {
			cerr << "polonius-reader: Error: Multiple files specified" << endl;
			return EXIT_BADFILE;
		}
		file_to_read = argv[option_index];
		received_filename = true;
	}
	
	// Make sure we got an input file
	if (!received_filename) {
		cerr << helpstring;
		return EXIT_BADFILE;
	}
	
	reader::file the_file;
	
	if (!the_file.init(file_to_read)) {
		cerr << program_name << ": " << the_file.get_init_error_message() << endl;
		return EXIT_BADFILE;
	}
	
	// If -l / --length wasn't set, just set it to the full length of the file
	if (amount_to_read == -1) {
		amount_to_read = the_file.get_file_length();
	}

	cout << the_file.read(start_position, amount_to_read);

	return EXIT_SUCCESS;
}
