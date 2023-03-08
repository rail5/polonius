#include "includes.h"

using namespace std;

int main(int argc, char* argv[]) {

	string program_name = "polonius-editor";
	
	string program_version = "0.1";
	
	string program_author = "rail5";
	
	string helpstring = "No help here\n";
	
	
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
	
	for (int i = 0; i < instructions_to_add.size(); i++) {
		if (!test_file.add_instruction(instructions_to_add[i])) {
			cerr << program_name << ": " << instructions_to_add[i].get_error_message() << endl;
			return 1;
		}
	}
	
	cout << endl << "DEBUG INFO:" << endl;
	cout << "  Object editor::file test_file = add_file('./testfile')" << endl;
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
	}
	return 0;

}
