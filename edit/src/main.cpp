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
	std::string program_name = "polonius-editor";
	
	std::string help_string =
	program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author + "\n\n"
	"This is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\n"
	""
	"Usage: " + program_name + " filename -a \"{INSTRUCTION}\"\n\n"
	""
	"Options:\n"
	" -i\n"
	" --input\n"
	"   Specify input file to edit\n\n"
	""
	"  -a\n"
	" --add-instruction\n"
	"   Instruct the program on how to edit your file\n"
	"     Example instructions:\n"
	"     REPLACE 5 hello world\n"
	"       (Replaces text, starting from character #5, with \"hello world\")\n"
	"     INSERT 7 salut a tous\n"
	"       (Inserts \"salut a tous\" at character #7, shifting the rest of the file without replacing it)\n"
	"     REMOVE 9 15\n"
	"       (Removes characters #9 to #15 from the file)\n\n"
	""
	"  -s\n"
	" --add-instruction-sequence\n"
	"   Provide a sequence of multiple instructions for editing the file\n"
	"     Each instruction in the sequence should be on its own line, as in the following example:\n"
	"       --add-instruction-sequence \"REPLACE 20 hello world\n"
	"       INSERT 50 hello again\n"
	"       REMOVE 70 75\"\n\n"
	""
	"  -f\n"
	" --add-instruction-file\n"
	"   Read our editing instructions from a file\n"
	"     Each instruction within the file should be on its own line, as with instruction sequences\n\n"
	""
	"  -c\n"
	" --special-chars\n"
	"   Interpret escaped character sequences (\\n, \\t and \\\\)\n\n"
	""
	"  -b\n"
	" --block-size\n"
	"   Specify the amount of data from the file we're willing to load into memory at any given time\n"
	"     Example:\n"
	"       -b 10M\n"
	"       -b 200K\n"
	"     (Default 10 kilobytes)\n\n"
	""
	"  -v\n"
	" --verbose\n"
	"   Verbose mode\n\n"
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
	"  " + program_name + " --input ./file.txt --add-instruction \"REPLACE 20 hello \\n"
	"world\" --add-instruction \"REMOVE 10 12\" --block-size 10K --special-chars\n\n"
	""
	"  " + program_name + " ./file.txt -f ./instructions.txt\n\n"
	""
	"  " + program_name + " -a \"insert 0 hello world\" ./file.txt\n";
	
	
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
				std::cout << program_version << std::endl;
				return EXIT_SUCCESS;
				break;
			
			case 'h':
				std::cout << help_string;
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
