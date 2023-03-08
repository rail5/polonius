using namespace std;

namespace editor {

	enum operation_type {
		no_operation,
		replace_operation,
		insert_operation,
		remove_operation
	};

	class instruction {
		private:
			bool initialized = false;
			
			operation_type operation = no_operation;
			/*
			Operation types:
				0 = none
				1 = replace
				2 = insert
				3 = remove
			*/
			int start_position = -1;
			int end_position = -1;
			string text_input = "";
			
			string error_message = "";
		public:
			void clear_instruction();
			bool set_replace_instruction(int start, string text);
			bool set_insert_instruction(int start, string text);
			bool set_remove_instruction(int start, int end);
			void set_error_message(string message);
			
			string get_error_message();
			
			bool is_initialized();
			int get_operation_type();
			int get_start_position();
			int get_end_position();
			string get_text();
	};
	
	instruction create_replace_instruction(int start_position, string text);
	instruction create_insert_instruction(int start_position, string text);
	instruction create_remove_instruction(int start_position, int end_position);
	
	instruction parse_instruction_string(string instruction_string);
	
	vector<instruction> parse_instruction_set_string(string instruction_set);

	class file {
		private:
			bool initialized = false;
			string file_name;
			string file_directory;
			int file_length = 0;
			vector<instruction> instruction_set;
		public:
			bool set_file(string file_path);
			bool insert(int start_position, string text_to_insert);
			bool replace(int start_position, string replacement_text);
			bool remove(int start_position, int end_position);
			bool add_instruction(instruction &input_instruction);
			bool execute_instructions();
			
			bool is_initialized();
			string get_file_name();
			string get_file_directory();
			int get_file_length();
			vector<instruction> get_instruction_set();
	};

	file add_file(string file_path);

}
