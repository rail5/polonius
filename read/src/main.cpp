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

	string help_string =
	program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author + "\n\n"
	"This is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\n"
	""
	"Usage: " + program_name + " -i filename\n\n"
	""
	"Options:\n"
	"  -i\n"
	"  --input\n"
	"    Specify input file to read\n\n"
	""
	"  -s\n"
	"  --start\n"
	"    Specify byte number to start reading from\n\n"
	""
	"  -l\n"
	"  --length\n"
	"   Specify how many bytes to read\n\n"
	""
	"  -b\n"
	" --block-size\n"
	"   Specify the amount of data from the file we're willing to load into memory at any given time\n"
	"     Example:\n"
	"       -b 10M\n"
	"       -b 200K\n"
	"     (Default 10 kilobytes)\n\n"
	""
	"  -f\n"
	" --find\n"
	" --search\n"
	"   Search for a string in the file\n"
	"   If -s / --start is specified, the program will only search for matches after that start position\n"
	"   If -l / --length is specified, the program will only search for matches within that length from the start\n"
	"   Returns nothing (blank) if no matches were found\n\n"
	""
	"  -p\n"
	" --output-pos\n"
	"   Output the start and end position, rather than the text (in the format \"start,end\", for example 10,15)\n"
	"   If used with searches, this will output the start and end position of the search result\n"
	"   Outside of searches, it will return the values of -s / --start and the end position (start + length)\n\n"
	""
	"  -c\n"
	"  --special-chars\n"
	"    Parse escaped character sequences in search queries (\\n, \\t, \\\\, and \\x00 through \\xFF)\n\n"
	""
	"  -V\n"
	" --version\n"
	"   Print version number\n\n"
	""
	"  -h\n"
	" --help\n"
	"   Display this message\n\n"
	""
	"Examples:\n"
	"  " + program_name + " --input ./file.txt --start 50 --length 10\n\n"
	""
	"  " + program_name + " -s 50 -l 10 ./file.txt\n\n"
	""
	"  " + program_name + " -i ./file.txt --start 75 --search \"hello world\" --length 10\n\n"
	""
	"  " + program_name + " ./file.txt -f \"hello world\" --output-pos\n";
	
	/*
	Necessary info for the program to do its job
	The file to read (-i),
	The byte to start from (-s) [0 if unspecified]
	The number of bytes to read (-l) [-1 if unspecified]
		"amount_to_read" == -1 will result in reading from the start position to the end of the file
	*/
	bool received_filename = false;
	string file_to_read = "";
	
	int64_t start_position = 0;
	int64_t amount_to_read = -1;
	
	int block_size = 10240;
	
	reader::job_type job = reader::read_job;
	string searching_for = "";
	
	bool output_position = false;

	bool special_chars = false;

	reader::search_type query_type = reader::normal_search;
	
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
		{"block-size", required_argument, 0, 'b'},
		{"find", required_argument, 0, 'f'},
		{"search", required_argument, 0, 'f'},
		{"output-pos", no_argument, 0, 'p'},
		{"special-chars", no_argument, 0, 'c'},
		{"regex", no_argument, 0, 'e'},
		{"version", no_argument, 0, 'V'},
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:l:b:f:pceVh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				if (received_filename) {
					cerr << program_name << ": Error: Multiple files specified" << endl;
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
				
			case 'b':
				block_size = parse_block_units(optarg);
				if (block_size == -1) {
					cerr << program_name << ": Block size '" << optarg << "' is not understood" << endl << "Use -h for help" << endl;
					return EXIT_BADARG;
				}
				break;
			
			case 'f':
				job = reader::search_job;
				searching_for = optarg;
				break;
			
			case 'p':
				output_position = true;
				break;
			
			case 'c':
				special_chars = true;
				break;
			
			case 'e':
				query_type = reader::regex_search;
				break;
				
			case 'V':
				cout << program_version << endl;
				return EXIT_SUCCESS;
				break;
				
			case 'h':
				cout << help_string;
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
			cerr << program_name << ": Error: Multiple files specified" << endl;
			return EXIT_BADFILE;
		}
		file_to_read = argv[option_index];
		received_filename = true;
	}
	
	// Make sure we got an input file
	if (!received_filename) {
		cerr << help_string;
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
	
	the_file.set_start_position(start_position);
	the_file.set_amount_to_read(amount_to_read);
	
	the_file.set_just_outputting_positions(output_position);
	
	the_file.set_block_size(block_size);

	if (special_chars) {
		searching_for = process_escapedchars(searching_for);
		searching_for = process_bytecodes(searching_for);
	}
	
	the_file.set_search_query(searching_for);

	the_file.set_search_type(query_type);
	
	the_file.set_job_type(job);

	the_file.do_job();

	return EXIT_SUCCESS;
}
