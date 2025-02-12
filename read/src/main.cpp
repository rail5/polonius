/***
 * Copyright (C) 2024 rail5
*/

#include "includes.h"

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
#endif

int main(int argc, char* argv[]) {
	const std::string program_name = "polonius-reader";

	const std::string help_string =
	program_name + " " + program_version
	"\n"
	"Usage: " + program_name + " [file] [options]\n"
	""
	"Options:\n"
	"  -i, --input [file]       File to read\n"
	"  -s, --start [byte]       Byte to start from (default: 0)\n"
	"  -l, --length [bytes]     Number of bytes to read (default: until EOF)\n"
	"  -b, --block-size [size]  Block size (default: 10K)\n"
	"  -f, --find [string]      Search for a string\n"
	"                             If -s is set, only search from that position\n"
	"                             If -l is set, only search that many bytes\n"
	"  -p, --output-pos         Output start and end position rather than text\n"
	"                             If -f is set, output position of the match\n"
	"  -c, --special-chars      Process escaped characters in the search string\n"
	"                             (\\n, \\t, \\\\, \\x00 through \\xFF)\n"
	"  -e, --regex              Treat the search string as a regular expression\n"
	"  -V, --version            Display version information\n"
	"  -h, --help               Display this help message\n"
	""
	"Examples:\n"
	"  " + program_name + " -i file.txt -s 10 -l 100\n"
	"  " + program_name + " -s 50 -f \"[a-zA-Z]{3}\" -e -p ./file.txt";

	const std::string version_string = 
	program_name + " " + program_version + "\n"
	"Copyright (C) 2023-2025 rail5\n"
	"\n"
	"This program is free software; you can redistribute it and/or modify\n"
	"it under the terms of the GNU General Public License as published by\n"
	"the Free Software Foundation; either version 3 of the License, or\n"
	"(at your option) any later version.\n"
	"\n"
	"This program is distributed in the hope that it will be useful,\n"
	"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
	"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
	"\n"
	"You should have received a copy of the GNU General Public License\n"
	"along with this program. If not, see http://www.gnu.org/licenses/.";
	
	/*
	Necessary info for the program to do its job
	The file to read (-i),
	The byte to start from (-s) [0 if unspecified]
	The number of bytes to read (-l) [-1 if unspecified]
		"amount_to_read" == -1 will result in reading from the start position to the end of the file
	*/
	bool received_filename = false;
	std::string file_to_read = "";
	
	int64_t start_position = 0;
	int64_t amount_to_read = -1;
	
	int64_t block_size = 10240;
	
	reader::job_type job = reader::read_job;
	std::string searching_for = "";
	
	bool output_position = false;

	bool special_chars = false;

	reader::search_type query_type = reader::t_normal_search;
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] = {
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
					std::cerr << program_name << ": Error: Multiple files specified" << std::endl;
					return EXIT_BADFILE;
				}
				file_to_read = optarg;
				received_filename = true;
				break;
				
			case 's':
				if (!is_number(optarg)) {
					std::cerr << program_name << ": '" << optarg << "' is not an integer" << std::endl << "Use -h for help" << std::endl;
					return EXIT_BADARG;
				}
				start_position = (int64_t)std::stoll(optarg);
				break;
				
			case 'l':
				if (!is_number(optarg)) {
					std::cerr << program_name << ": '" << optarg << "' is not an integer" << std::endl << "Use -h for help" << std::endl;
					return EXIT_BADARG;
				}
				amount_to_read = (int64_t)std::stoll(optarg);
				break;
				
			case 'b':
				block_size = parse_block_units(optarg);
				if (block_size < 0) {
					std::cerr << program_name << ": Block size '" << optarg << "' is not understood or too large" << std::endl << "Use -h for help" << std::endl;
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
				query_type = reader::t_regex_search;
				break;
				
			case 'V':
				std::cout << version_string << std::endl;
				return EXIT_SUCCESS;
				break;
				
			case 'h':
				std::cout << help_string << std::endl;
				return EXIT_SUCCESS;
				break;
				
			case '?':
				if (optopt == 'i' || optopt == 's' || optopt == 'l') {
					std::cerr << program_name << ": Option -" << static_cast<char>(optopt) << " requires an argument" << std::endl << "Use -h for help" << std::endl;
					return EXIT_BADOPT;
				}
				break;
		}
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		if (received_filename) {
			std::cerr << program_name << ": Error: Multiple files specified" << std::endl;
			return EXIT_BADFILE;
		}
		file_to_read = argv[option_index];
		received_filename = true;
	}
	
	// Make sure we got an input file
	if (!received_filename) {
		std::cerr << help_string << std::endl;
		return EXIT_BADFILE;
	}
	
	reader::file the_file;
	
	if (!the_file.init(file_to_read)) {
		std::cerr << program_name << ": " << the_file.get_init_error_message() << std::endl;
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

	if (the_file.do_job()) {
		return EXIT_SUCCESS;
	} else {
		return EXIT_OTHER;
	}
}
