/***
 * Copyright (C) 2024 rail5
*/

#ifndef EDIT_SRC_NAMESPACE_EDITOR_H_
#define EDIT_SRC_NAMESPACE_EDITOR_H_

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
		std::string text_input = "";
		
		std::string error_message = "";

	public:
		void clear_instruction();
		void process_special_chars();
		bool set_replace_instruction(int64_t start, std::string text);
		bool set_insert_instruction(int64_t start, std::string text);
		bool set_remove_instruction(int64_t start, int64_t end);
		bool set_error_message(std::string message, operation_type instruction_type = no_operation, int64_t start = 0, int64_t end = 0, std::string input = "");
		
		std::string get_error_message();

		void update_start_position(int64_t start);
		void update_end_position(int64_t end);
		
		bool is_initialized() const;
		operation_type get_operation_type() const;
		int64_t get_start_position() const;
		int64_t get_end_position() const;
		std::string get_text() const;
		std::string get_formatted_text() const;

		friend std::ostream& operator<<(std::ostream& output_stream, const instruction& input) {
			std::string output_string = "";
			std::string second_part = "";
			std::string third_part = "";
			switch (input.operation) {
				case replace_operation:
					output_string += "REPLACE";
					break;
				case insert_operation:
					output_string += "INSERT";
					break;
				case remove_operation:
					output_string += "REMOVE";
					if (input.end_position == -1) {
						third_part = "end";
					} else {
						third_part = std::to_string(input.end_position);
					}
					break;
				default:
					return output_stream << "INVALID INSTRUCTION";
			}

			if (input.start_position == -1) {
				second_part = "end";
			} else {
				second_part = std::to_string(input.start_position);
			}

			if (third_part == "") {
				// REPLACE or INSERT operation
				// Get and format the insertion/replacement text
				third_part = input.get_formatted_text();
			}

			output_string += " " + second_part + " " + third_part;

			return output_stream << output_string;
		}
};

instruction create_replace_instruction(int64_t start_position, std::string text);
instruction create_insert_instruction(int64_t start_position, std::string text);
instruction create_remove_instruction(int64_t start_position, int64_t end_position);

std::vector<instruction> parse_instruction_file(std::string file_path);

std::vector<instruction> parse_instruction_sequence_string(std::string instruction_sequence);

std::vector<instruction> parse_instruction_line(std::string instruction_line);

instruction parse_instruction_string(std::string instruction_string);

class file {
	private:
		bool initialized = false;
		bool file_is_new = false;
		
		std::string file_name;
		std::string file_directory;
		
		int64_t file_length = 0;
		
		std::fstream file_stream;
		FILE* c_type_file;
		int file_descriptor;
		
		int64_t block_size = 10240;
		
		std::vector<instruction> instruction_sequence;
		int64_t file_length_after_last_instruction = 0;
		
		std::string error_message = "";
		
		bool verbose = false;

	public:
		bool set_file(std::string file_path);
		
		void set_block_size(int64_t specified_blocksize);
		
		void replace(int64_t start_position, std::string replacement_text);
		void insert(int64_t start_position, std::string text_to_insert);
		void remove(int64_t start_position, int64_t end_position);

		bool add_instruction(instruction &input_instruction);
		
		bool execute_single_instruction(instruction instruction_to_execute);
		
		bool execute_instructions();
		
		bool is_initialized();
		
		std::string get_file_name();
		std::string get_file_directory();
		
		int64_t get_block_size();
		
		int64_t get_file_length();
		
		std::vector<instruction> get_instruction_sequence();
		
		std::string get_error_message();
		
		void close();
		void clean_up();
		
		/*
		Constructor
		*/
		explicit file(std::string path, int64_t blocksize = 10240, bool verbose_mode = false);
};
} // namespace editor

#endif // EDIT_SRC_NAMESPACE_EDITOR_H_
