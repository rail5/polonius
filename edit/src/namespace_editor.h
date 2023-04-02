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
			
			int64_t start_position = -1;
			int64_t end_position = -1;
			string text_input = "";
			
			string error_message = "";
		public:
			void clear_instruction();
			void process_special_chars();
			bool set_replace_instruction(int64_t start, string text);
			bool set_insert_instruction(int64_t start, string text);
			bool set_remove_instruction(int64_t start, int64_t end);
			void set_error_message(string message);
			
			string get_error_message();
			
			bool is_initialized();
			int get_operation_type();
			int64_t get_start_position();
			int64_t get_end_position();
			string get_text();

	};
	
	instruction create_replace_instruction(int64_t start_position, string text);
	instruction create_insert_instruction(int64_t start_position, string text);
	instruction create_remove_instruction(int64_t start_position, int64_t end_position);
	
	instruction parse_instruction_string(string instruction_string);
	
	vector<instruction> parse_instruction_set_string(string instruction_set);

	class file {
		private:
			bool initialized = false;
			
			string file_name;
			string file_directory;
			
			int64_t file_length = 0;
			
			fstream file_stream;
			FILE* c_type_file;
			int file_descriptor;
			
			int block_size = 1024;
			
			vector<instruction> instruction_set;
			int64_t file_length_after_last_instruction = 0;
			
			string error_message = "";
			
			bool verbose = false;
		public:
			bool set_file(string file_path);
			
			void set_block_size(int specified_blocksize);
			
			void replace(int64_t start_position, string replacement_text);
			void insert(int64_t start_position, string text_to_insert);
			void remove(int64_t start_position, int64_t end_position);
			
			bool add_instruction(instruction &input_instruction);
			
			bool execute_single_instruction(instruction instruction_to_execute);
			
			bool execute_instructions();
			
			bool is_initialized();
			
			string get_file_name();
			string get_file_directory();
			
			int get_block_size();
			
			int64_t get_file_length();
			
			vector<instruction> get_instruction_set();
			
			string get_error_message();
			
			void close();
			
			/*
			Constructor
			*/
			file(string path, int blocksize = 1024, bool verbose_mode = false);
	};

}
