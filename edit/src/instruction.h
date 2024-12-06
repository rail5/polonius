/***
 * Copyright (C) 2024 rail5
*/

#ifndef EDIT_SRC_INSTRUCTION_H_
#define EDIT_SRC_INSTRUCTION_H_

#include <cstdint>
#include <string>
#include <list>
#include <memory>
#include <ostream>

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
						third_part = std::to_string(input.end_position - 1);
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

std::list<instruction> parse_instruction_file(std::string file_path);
std::list<instruction> parse_instruction_sequence_string(std::string instruction_sequence);
std::list<instruction> parse_instruction_line(std::string instruction_line);
instruction parse_instruction_string(std::string instruction_string);

/* Instruction optimization */
std::list<instruction> optimize_instruction_sequence(const std::list<instruction>& instruction_sequence);
void combine_inserts(std::shared_ptr<std::list<instruction>> instruction_sequence, std::list<editor::instruction>::iterator input_iterator);
} // namespace editor

#endif // EDIT_SRC_INSTRUCTION_H_