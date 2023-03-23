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
	
	string program_version = "0.1";
	
	string program_author = "rail5";
	
	string helpstring = program_name + " " + program_version + "\nCopyright (C) 2023 " + program_author +"\n\nThis is free software (GNU GPL 3), and you are welcome to redistribute it under certain conditions.\n\nUsage: " + program_name + " -i filename -a \"{INSTRUCTION}\"\n\nOptions:\n  -i\n  --input\n    Specify input file to edit\n\n  -a\n  --add-instruction\n    Instruct the program on how to edit your file\n      Example instructions:\n      REPLACE 5 hello world\n        (Replaces text, starting from byte #5, with \"hello world\")\n      INSERT 7 salut a tous\n        (Inserts \"salut a tous\" at byte #7, shifting the rest of the file without replacing it)\n      REMOVE 9 15\n        (Removes bytes #9 to #15 from the file)\n\n  -s\n  --add-instruction-set\n    Provide a set of multiple instructions for editing the file\n      Each instruction in the set should be on its own line, as in the following example:\n        --add-instruction-set \"REPLACE 20 hello world\n        INSERT 50 hello again\n        REMOVE 70 75\"\n\n  -c\n  --special-chars\n    Interpret escaped character sequences (\\n, \\t and \\\\)\n\n  -f\n  --follow-logic\n    Adjust the positions given by the instruction set as we go along\n      For example, in this set:\n        REMOVE 5 5\n        REPLACE 10 hello world\n      After \"Remove 5 5\", which removes the 5th byte from the file, \"Replace 10 hello world\" will be adjusted to \"Replace 9 hello world\", since removing an earlier byte caused the text which was originally at position #10 to move to position #9\n\n  -b\n  --block-size\n    Specify the amount of data from the file we're willing to load into memory at any given time\n      (Specified in bytes)\n      (Default 1024)\n\n  -h\n  --help\n    Display this message\n\nExample:\n  " + program_name + " --input ./file.txt --add-instruction \"REPLACE 20 hello \\n world\" --add-instruction \"REMOVE 10 12\" --block-size 10240 --special-chars\n";
	
	
	/*
	Necessary information for the program to do its job
	*/
	string file_to_edit = "";
	vector<editor::instruction> instructions_to_add;
	int block_size = 1024;
	bool interpret_special_chars = false;
	bool follow_position_logic = false;
	
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
		{"follow-logic", no_argument, 0, 'f'},
		
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:s:a:b:cfh", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				file_to_edit = optarg;
				break;
			
			case 'a':
				instructions_to_add.push_back(parse_instruction_string(optarg));
				break;
			
			case 's':
				temp_instruction_set = parse_instruction_set_string(optarg);
				move(temp_instruction_set.begin(), temp_instruction_set.end(), inserter(instructions_to_add, instructions_to_add.end()));
				break;
			
			case 'b':
				if (!is_number(optarg)) {
					cerr << program_name << ": Error: '" << optarg << "' is not an integer" << endl << "Use -h for help" << endl;
					return 1;
				}
				block_size = (int)stol(optarg);
				break;
			
			case 'c':
				interpret_special_chars = true;
				break;
			
			case 'f':
				follow_position_logic = true;
				break;
			
			case 'h':
				cout << helpstring;
				return 0;
				break;
			
			case '?':
				if (optopt == 'i' || optopt == 's' || optopt == 'a' || optopt == 'b') {
					cerr << program_name << ": Option -" << (char)optopt << " requires an argument" << endl << "Use -h for help" << endl;
					return 1;
				}
				break;
		}
	}
	
	if (file_to_edit == "") {
		cerr << program_name << ": No input file given. Use -h for help" << endl;
		return 1;
	}

	editor::file test_file = add_file(file_to_edit);
	
	test_file.set_block_size(block_size);
	
	for (int i = 0; i < instructions_to_add.size(); i++) {
	
		if (interpret_special_chars) {
			instructions_to_add[i].process_special_chars();
		}
		
		if (!test_file.add_instruction(instructions_to_add[i])) {
			cerr << program_name << ": " << instructions_to_add[i].get_error_message() << endl;
			return 1;
		}
	}
	
	cout << endl << "DEBUG INFO:" << endl;
	cout << "  Object editor::file test_file = add_file('" << file_to_edit << "')" << endl;
	cout << "    initialized  =  " << test_file.is_initialized() << endl;
	cout << "    file_name  =  " << test_file.get_file_name() << endl;
	cout << "    file_directory   =  " << test_file.get_file_directory() << endl;
	cout << "    file_length  =  " << test_file.get_file_length() << endl << endl;
	
	vector<editor::instruction> instruction_set = test_file.get_instruction_set();
	
	for (int i = 0; i < instruction_set.size(); i++) {
		cout << "  Object INSTRUCTION #" << i << endl;
		cout << "    initialized  =  " << instruction_set[i].is_initialized() << endl;
		cout << "    operation_type  =  " << instruction_set[i].get_operation_type() << endl;
		cout << "    start_position  =  " << instruction_set[i].get_start_position() << endl;
		cout << "    end_position  =  " << instruction_set[i].get_end_position() << endl;
		cout << "    text_input  =  " << instruction_set[i].get_text() << endl;
		cout << "    error_message  =  " << instruction_set[i].get_error_message() << endl;
		test_file.execute_single_instruction(instruction_set[i]);
	}
	
	/* Close file */
	test_file.close();
	return 0;

}
