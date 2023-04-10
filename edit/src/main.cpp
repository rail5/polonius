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

	string program_name = "polonius-editor";
	
	string program_version = "0.3";
	
	string program_author = "rail5";
	
	string helpstring = program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author + "\n\nThis is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\nUsage: " + program_name + " -i filename -a \"{INSTRUCTION}\"\n\nOptions:\n  -i\n  --input\n    Specify input file to edit\n\n  -a\n  --add-instruction\n    Instruct the program on how to edit your file\n      Example instructions:\n      REPLACE 5 hello world\n        (Replaces text, starting from byte #5, with \"hello world\")\n      INSERT 7 salut a tous\n        (Inserts \"salut a tous\" at byte #7, shifting the rest of the file without replacing it)\n      REMOVE 9 15\n        (Removes bytes #9 to #15 from the file)\n\n  -s\n  --add-instruction-set\n    Provide a set of multiple instructions for editing the file\n      Each instruction in the set should be on its own line, as in the following example:\n        --add-instruction-set \"REPLACE 20 hello world\n        INSERT 50 hello again\n        REMOVE 70 75\"\n\n  -c\n  --special-chars\n    Interpret escaped character sequences (\\n, \\t and \\\\)\n\n  -b\n  --block-size\n    Specify the amount of data from the file we're willing to load into memory at any given time\n      Example:\n        -b 10M\n        -b 200K\n      (Default 10 kilobytes)\n\n  -v\n  --verbose\n    Verbose mode\n\n  -V\n  --version\n    Print version number\n\n  -h\n  --help\n    Display this message\n\nExamples:\n  " + program_name + " --input ./file.txt --add-instruction \"REPLACE 20 hello \\n world\" --add-instruction \"REMOVE 10 12\" --block-size 10K --special-chars\n\n  " + program_name + " -a \"insert 0 hello world\" ./file.txt\n";
	
	
	/*
	Necessary information for the program to do its job
	*/
	string file_to_edit = "";
	bool received_filename = false;
	
	vector<editor::instruction> instructions_to_add;
	int block_size = 10240;
	bool interpret_special_chars = false;
	
	bool verbose = false;
	
	/*
	temp_instruction_set vector
	-s / --add-instruction-set option fills this vector, moves its elements to instructions_to_add, and then clears this vector
	*/
	vector<editor::instruction> temp_instruction_set;
	
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] =
	{
		{"input", required_argument, 0, 'i'},
		
		{"add-instruction-set", required_argument, 0, 's'},
		{"instruction-set", required_argument, 0, 's'},
		
		{"add-instruction", required_argument, 0, 'a'},
		{"instruction", required_argument, 0, 'a'},
		
		{"block-size", required_argument, 0, 'b'},
		
		{"special-chars", no_argument, 0, 'c'},
		
		{"verbose", no_argument, 0, 'v'},
		
		{"version", no_argument, 0, 'V'},
		
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:a:b:cvVh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				if (received_filename) {
					cerr << "polonius-editor: Error: Multiple files specified" << endl;
					return EXIT_BADFILE;
				}
				file_to_edit = optarg;
				received_filename = true;
				break;
			
			case 'a':
				instructions_to_add.push_back(parse_instruction_string(optarg));
				break;
			
			case 's':
				temp_instruction_set = parse_instruction_set_string(optarg);
				move(temp_instruction_set.begin(), temp_instruction_set.end(), inserter(instructions_to_add, instructions_to_add.end()));
				break;
			
			case 'b':
				block_size = parse_block_units(optarg);
				if (block_size == -1) {
					cerr << program_name << ": Block size '" << optarg << "' is not understood" << endl << "Use -h for help" << endl;
					return EXIT_BADARG;
				}
				break;
			
			case 'c':
				interpret_special_chars = true;
				break;
			
			case 'v':
				verbose = true;
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
				if (optopt == 'i' || optopt == 's' || optopt == 'a' || optopt == 'b') {
					cerr << program_name << ": Option -" << (char)optopt << " requires an argument" << endl << "Use -h for help" << endl;
					return EXIT_BADOPT;
				}
				break;
		}
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		if (received_filename) {
			cerr << "polonius-editor: Error: Multiple files specified" << endl;
			return EXIT_BADFILE;
		}
		file_to_edit = argv[option_index];
		received_filename = true;
	}
	
	if (!received_filename) {
		cerr << program_name << ": No input file given. Use -h for help" << endl;
		return EXIT_BADFILE;
	}

	editor::file document(file_to_edit, block_size, verbose);
	
	if (!document.is_initialized()) {
		cerr << program_name << ": " << document.get_error_message() << endl;
		return EXIT_OTHER;
	}
	
	for (int i = 0; i < instructions_to_add.size(); i++) {
	
		if (interpret_special_chars) {
			instructions_to_add[i].process_special_chars();
		}
		
		if (!document.add_instruction(instructions_to_add[i])) {
			cerr << program_name << ": " << instructions_to_add[i].get_error_message() << endl;
			return EXIT_BADARG;
		}
	}
	
	vector<editor::instruction> instruction_set = document.get_instruction_set();
	
	for (int i = 0; i < instruction_set.size(); i++) {
		document.execute_single_instruction(instruction_set[i]);
	}
	
	/* Close file */
	document.close();
	return EXIT_SUCCESS;

}
