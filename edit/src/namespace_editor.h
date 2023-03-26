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
			
			long long int start_position = -1;
			long long int end_position = -1;
			string text_input = "";
			
			string error_message = "";
		public:
			void clear_instruction();
			void process_special_chars();
			bool set_replace_instruction(long long int start, string text);
			bool set_insert_instruction(long long int start, string text);
			bool set_remove_instruction(long long int start, long long int end);
			void set_error_message(string message);
			
			string get_error_message();
			
			bool is_initialized();
			int get_operation_type();
			long long int get_start_position();
			long long int get_end_position();
			string get_text();

	};
	
	instruction create_replace_instruction(long long int start_position, string text);
	instruction create_insert_instruction(long long int start_position, string text);
	instruction create_remove_instruction(long long int start_position, long long int end_position);
	
	instruction parse_instruction_string(string instruction_string);
	
	vector<instruction> parse_instruction_set_string(string instruction_set);

	class file {
		private:
			bool initialized = false;
			string file_name;
			string file_directory;
			long long int file_length = 0;
			int block_size = 1024;
			vector<instruction> instruction_set;
			fstream file_stream;
			FILE* c_type_file;
			int file_descriptor;
			string error_message = "";
		public:
			bool set_file(string file_path);
			void set_block_size(int specified_blocksize);
			void replace(long long int start_position, string replacement_text);
			void insert(long long int start_position, string text_to_insert);
			void remove(long long int start_position, long long int end_position);
			bool add_instruction(instruction &input_instruction);
			bool execute_single_instruction(instruction instruction_to_execute);
			bool execute_instructions();
			
			bool is_initialized();
			string get_file_name();
			string get_file_directory();
			int get_block_size();
			long long int get_file_length();
			vector<instruction> get_instruction_set();
			
			string get_error_message();
			
			void close();
	};

	file add_file(string file_path);

}
