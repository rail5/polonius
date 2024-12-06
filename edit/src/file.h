/***
 * Copyright (C) 2024 rail5
*/

#ifndef EDIT_SRC_FILE_H_
#define EDIT_SRC_FILE_H_

#include <string>
#include <fstream>
#include <list>

#include "instruction.h"

namespace editor {

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
		
		std::list<instruction> instruction_sequence;
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
		
		std::list<instruction> get_instruction_sequence();
		
		std::string get_error_message();
		
		void close();
		void clean_up();
		
		/*
		Constructor
		*/
		explicit file(std::string path, int64_t blocksize = 10240, bool verbose_mode = false);
};
} // namespace editor

#endif // EDIT_SRC_FILE_H_
