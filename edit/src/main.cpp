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
	const std::string program_name = "polonius-editor";
	
	const std::string help_string =
	program_name + " " + program_version + "\n"
	"Usage: " + program_name + " [file] [options]\n"
	""
	"Options:\n"
	"  -i, --input [file]                    File to edit\n"
	"  -a, --add-instruction [instruction]   Add an instruction\n"
	"                                         (format: [REPLACE|INSERT|REMOVE] [start] [text|end])\n"
	"                                         See the manual for more information\n"
	"  -s, --add-instruction-sequence [seq]  Add an instruction sequence\n"
	"                                         (one instruction per line)\n"
	"  -f, --add-instruction-file [file]     Add instructions from a file\n"
	"                                         (one instruction per line)\n"
	"  -c, --special-chars                   Process escaped chars in instructions\n"
	"                                         (\\n, \\t, \\\\, \\x00 through \\xFF)\n"
	"  -b, --block-size [size]               Block size (default: 10K)\n"
	"  -v, --verbose                         Output verbose information\n"
	"  -V, --version                         Display version information\n"
	"  -h, --help                            Display this help message\n"
	""
	"Examples:\n"
	"  " + program_name + " -i file.txt -a REPLACE 5 hello\n"
	"  " + program_name + " ./file.txt -a \"INSERT 0 new text for start of file\"";
	
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
	Necessary information for the program to do its job
	*/
	std::string file_to_edit = "";
	bool received_filename = false;
	
	std::vector<editor::instruction> instructions_to_add;
	int64_t block_size = 10240;
	bool interpret_special_chars = false;
	
	bool verbose = false;

	bool just_get_optimized_instructions = false;
	
	/*
	temp_instruction_sequence std::vector
	-s / --add-instruction-sequence option fills this std::vector, moves its elements to instructions_to_add, and then clears this std::vector
	*/
	std::vector<editor::instruction> temp_instruction_sequence;
	
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] = {
		{"input", required_argument, 0, 'i'},
		
		{"add-instruction-sequence", required_argument, 0, 's'},
		{"instruction-sequence", required_argument, 0, 's'},
		
		{"add-instruction", required_argument, 0, 'a'},
		{"instruction", required_argument, 0, 'a'},

		{"add-instruction-file", required_argument, 0, 'f'},
		{"instruction-file", required_argument, 0, 'f'},

		{"get-optimized-instructions", no_argument, 0, 'g'},
		
		{"block-size", required_argument, 0, 'b'},
		
		{"special-chars", no_argument, 0, 'c'},
		
		{"verbose", no_argument, 0, 'v'},
		
		{"version", no_argument, 0, 'V'},
		
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:a:f:b:cgvVh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				if (received_filename) {
					std::cerr << program_name << ": Error: Multiple files specified" << std::endl;
					return EXIT_BADFILE;
				}
				file_to_edit = optarg;
				received_filename = true;
				break;
			
			case 'a':
				temp_instruction_sequence = parse_instruction_line(optarg);
				move(temp_instruction_sequence.begin(), temp_instruction_sequence.end(), inserter(instructions_to_add, instructions_to_add.end()));
				break;
			
			case 's':
				temp_instruction_sequence = parse_instruction_sequence_string(optarg);
				move(temp_instruction_sequence.begin(), temp_instruction_sequence.end(), inserter(instructions_to_add, instructions_to_add.end()));
				break;
			
			case 'f':
				temp_instruction_sequence = parse_instruction_file(optarg);
				move(temp_instruction_sequence.begin(), temp_instruction_sequence.end(), inserter(instructions_to_add, instructions_to_add.end()));
				break;
			
			case 'b':
				block_size = parse_block_units(optarg);
				if (block_size < 0) {
					std::cerr << program_name << ": Block size '" << optarg << "' is not understood or too large" << std::endl << "Use -h for help" << std::endl;
					return EXIT_BADARG;
				}
				break;
			
			case 'c':
				interpret_special_chars = true;
				break;
			
			case 'g':
				just_get_optimized_instructions = true;
				break;
			
			case 'v':
				verbose = true;
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
				if (optopt == 'i' || optopt == 's' || optopt == 'a' || optopt == 'b') {
					std::cerr << program_name << ": Option -" << static_cast<char>(optopt) << " requires an argument" << std::endl << "Use -h for help" << std::endl;
					return EXIT_BADOPT;
				}
				break;
		}
	}

	if (instructions_to_add.empty()) {
		std::cerr << program_name << ": No instructions given. Use -h for help" << std::endl;
		return EXIT_BADARG;
	}

	if (just_get_optimized_instructions) {
		if (interpret_special_chars) {
			for (editor::instruction& original_instruction : instructions_to_add) {
				original_instruction.process_special_chars();
			}
		}
		std::vector<editor::instruction> optimized = optimize_instruction_sequence(instructions_to_add);
		for (editor::instruction optimized_instruction : optimized) {
			std::cout << optimized_instruction << std::endl;
		}
		return 0;
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		if (received_filename) {
			std::cerr << program_name << ": Error: Multiple files specified" << std::endl;
			return EXIT_BADFILE;
		}
		file_to_edit = argv[option_index];
		received_filename = true;
	}
	
	if (!received_filename) {
		std::cerr << program_name << ": No input file given. Use -h for help" << std::endl;
		return EXIT_BADFILE;
	}

	editor::file document(file_to_edit, block_size, verbose);
	
	if (!document.is_initialized()) {
		std::cerr << program_name << ": " << document.get_error_message() << std::endl;
		return EXIT_OTHER;
	}
	
	for (int i = 0; i < instructions_to_add.size(); i++) {
		if (interpret_special_chars) {
			instructions_to_add[i].process_special_chars();
		}
		
		if (!document.add_instruction(instructions_to_add[i])) {
			std::cerr << program_name << ": " << instructions_to_add[i].get_error_message() << std::endl;
			document.clean_up();
			return EXIT_BADARG;
		}
	}
	
	std::vector<editor::instruction> instruction_sequence = document.get_instruction_sequence();
	
	for (int i = 0; i < instruction_sequence.size(); i++) {
		document.execute_single_instruction(instruction_sequence[i]);
	}
	
	/* Close file */
	document.close();
	return EXIT_SUCCESS;
}
