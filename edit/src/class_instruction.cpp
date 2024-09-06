/***
 * Copyright (C) 2024 rail5
*/

void editor::instruction::clear_instruction() {
	/***
	void clear_instruction():
		- Sets instruction to unitialized & clears variables
	***/
	
	initialized = false;
	operation = no_operation;
	start_position = -1;
	end_position = -1;
	text_input = "";
}

void editor::instruction::process_special_chars() {
	if (operation == remove_operation || operation == no_operation) {
		return;
	}

	/* Process \n, \t, and \\ */
	text_input = process_escapedchars(text_input);

	/* Process \x00 through \xFF */
	text_input = process_bytecodes(text_input);
	
	end_position = (start_position + text_input.length());
}

bool editor::instruction::set_replace_instruction(int64_t start, std::string text) {
	/***
	bool set_replace_instruction():
		- Sets the current instruction to type "replace"
		- Sets relevant variables
	Returns true if successful
	Runs clear_instruction & returns false if failure
	***/

	/*
	Make sure the start position is valid.
	Meaning:
		- Not less than -1 (-1 is reserved for the 'end' keyword)
		- Not beyond EOF
	*/

	if (start < -1) {
		set_error_message("Invalid start position", replace_operation, start, 0, text);
		clear_instruction();
		return false;
	}
	
	/*
	Set object 'private' info to user-provided parameters
	And then declare the object initialized
	*/
	operation = replace_operation;
	start_position = start;
	end_position = (start + text.length());
	text_input = text;
	initialized = true;
	
	/* Clear any earlier error messages */
	error_message = "";
	
	return true;
}

bool editor::instruction::set_insert_instruction(int64_t start, std::string text) {
	/***
	bool set_insert_instruction():
		- Sets the current instruction to type "insert"
		- Sets relevant variables
	Returns true if successful
	Runs clear_instruction & returns false if failure
	***/

	/*
	Make sure the start position is valid
	Meaning:
		- Not less than -1 (-1 is reserved for the 'end' keyword)
		- Not beyond EOF
	*/
	if (start < -1) {
		set_error_message("Invalid start position", insert_operation, start, 0, text);
		clear_instruction();
		return false;
	}
	
	/*
	Set object 'private' info to user-provided parameters
	And then declare the object initialized
	*/
	operation = insert_operation;
	start_position = start;
	end_position = (start + text.length());
	text_input = text;
	initialized = true;
	
	/* Clear any earlier error messages */
	error_message = "";
	
	return true;
}

bool editor::instruction::set_remove_instruction(int64_t start, int64_t end) {
	/***
	bool set_remove_instruction():
		- Sets the current instruction to type "remove"
		- Sets relevant variables
	Returns true if successful
	Runs clear_instruction & returns false if failure
	***/

	/*
	Make sure the start position is valid
	Meaning:
		- Not less than -1 (-1 is reserved for the 'end' keyword)
	*/
	if (start < -1) {
		set_error_message("Invalid start position", remove_operation, start, end, "");
		clear_instruction();
		return false;
	}
	
	/*
	Make sure the end position is valid
	Meaning:
		- Not less than the start position
	*/
	if ( 
		((end < start) && (end != -1))
		||
		((start == -1) && (end != -1)) /* Disallow the start position being 'END' and the end position not being 'END' */
	) {
		set_error_message("Invalid end position", remove_operation, start, end, "");
		clear_instruction();
		return false;
	}
	
	/*
	Set object 'private' info to user-provided parameters
	And then declare the object initialized
	*/
	operation = remove_operation;
	start_position = start;
	if (end != -1) {
		// '-1' is set if the user used the 'end' keyword
		// Later parts of the program will check if start/end positions are set to -1,
		// And if so, dynamically update them to point to the end of the file
		// So, here we don't want to increment '-1' to '0'
		// But, for all other points, remove instruction positions need to be updated INTERNALLY
		// So that the end position is one beyond the point we want to remove
		end_position = end + 1;
	} else {
		end_position = end;
	}
	initialized = true;
	
	/* Clear any earlier error messages */
	error_message = "";
	
	return true;
}

bool editor::instruction::set_error_message(std::string message, operation_type instruction_type, int64_t start, int64_t end, std::string input) {
	/***
	void set_error_message(std::string message, operation_type instruction_type, int64_t start, int64_t end, std::string input):
		Set the instruction's "error_message" to the inputted string, with the option to provide more information
	***/

	std::string extra_info = " ";

	switch (instruction_type) {
		case no_operation:
			error_message = message; // Just provide 'message', no additional information
			return true;
			break;

		case replace_operation:
			extra_info += "(REPLACE " + std::to_string(start) + " " + input + ")";
			break;

		case insert_operation:
			extra_info += "(INSERT " + std::to_string(start) + " " + input + ")";
			break;

		case remove_operation:
			extra_info += "(REMOVE " + std::to_string(start) + " " + std::to_string(end) + ")";
			break;

		default:
			return false; // Error!
			break;
	}

	error_message = message + extra_info;
	return true;
}

std::string editor::instruction::get_error_message() {
	return error_message;
}

void editor::instruction::update_start_position(int64_t start) {
	start_position = start;
}

void editor::instruction::update_end_position(int64_t end) {
	end_position = end;
}

bool editor::instruction::is_initialized() {
	return initialized;
}

editor::operation_type editor::instruction::get_operation_type() {
	return operation;
}

int64_t editor::instruction::get_start_position() {
	return start_position;
}

int64_t editor::instruction::get_end_position() {
	return end_position;
}

std::string editor::instruction::get_text() {
	return text_input;
}

editor::instruction create_replace_instruction(int64_t start_position, std::string text) {
	editor::instruction new_instruction;
	
	new_instruction.set_replace_instruction(start_position, text);
	
	return new_instruction;
}

editor::instruction create_insert_instruction(int64_t start_position, std::string text) {
	editor::instruction new_instruction;
	
	new_instruction.set_insert_instruction(start_position, text);
	
	return new_instruction;
}

editor::instruction create_remove_instruction(int64_t start_position, int64_t end_position) {
	editor::instruction new_instruction;
	
	new_instruction.set_remove_instruction(start_position, end_position);
	
	return new_instruction;
}


editor::instruction parse_instruction_string(std::string instruction_string) {
	/*
	instruction parse_instruction_string(std::string instruction_string):
		Create an 'instruction' object from a properly-formatted string
		
		The std::string must be formatted in one of the following ways:
			1.
				REPLACE 5 hello
					(
					This would replace characters #5 - #9 with "hello"
						Example:
							Original file:
								01234567890123
							New file:
								01234hello0123
					)
			2.
				INSERT 6 ni hao
					(
					This would shift characters #6 - EOF rightward and insert "ni hao" at position #6 without replacing
					(Or, if #6 is EOF, this would insert "ni hao" at the end of the file)
						Example:
							Original file:
								01234567890123
							New file:
								012345ni hao67890123
					)
			3.
				REMOVE 7 10
					(
					This would remove characters #7 - #10 and shift the remaining characters leftward
						Example:
							Original file:
								01234567890123
							New file:
								0123456123
					)
		That is, the std::string must be formatted with these rules:
			- Space-delimited
			- The first key is the instruction name (REPLACE, INSERT, or REMOVE)
			- The second key is the start position
			- The third key is either:
				- A std::string (in the case of REPLACE or INSERT)
				- The end position (in the case of REMOVE)
	*/
	
	/*
	Remove leading whitespace
	*/
	
	instruction_string = remove_leading_whitespace(instruction_string);
	
	/*
	Split the std::string into a std::vector<std::string> delimited by spaces
		See: shared_functions/explode.cpp
	*/
	std::vector<std::string> instruction_vector = explode(instruction_string, ' ', false, 3);
	
	/*
	Set up an 'instruction' object marked invalid to return if there's a problem
	*/
	editor::instruction invalid_instruction;
	
	/*
	Make sure we have exactly 3 elements
	*/
	if (instruction_vector.size() != 3) {
		invalid_instruction.set_error_message("Invalid instruction: " + instruction_string);
		return invalid_instruction;
	}

	std::string first_element = "";
	std::string second_element = "";
	std::string third_element = "";

	bool second_element_is_end = false;
	bool third_element_is_end = false;
	
	/*
	Make sure the first element is either "replace," "insert," or "remove"
	*/
	first_element = to_lower(instruction_vector[0]);
	bool is_replace_instruction = (first_element == "replace");
	bool is_insert_instruction = (first_element == "insert");
	bool is_remove_instruction = (first_element == "remove");
	
	bool instruction_type_is_valid = (is_replace_instruction || is_insert_instruction || is_remove_instruction);
	
	if (!instruction_type_is_valid) {
		invalid_instruction.set_error_message("Invalid instruction type: '" + instruction_vector[0] + "'");
		return invalid_instruction;
	}
	
	/*
	The second element should ALWAYS be either an integer or the keyword "end"
		See: shared_functions/is_number.cpp
	*/
	second_element = to_lower(instruction_vector[1]);

	second_element_is_end = (second_element == "end");

	if ( !(is_number(second_element) || (second_element_is_end)) ) {
		invalid_instruction.set_error_message("Invalid instruction: '" + instruction_vector[1] + "' is not a positive integer");
		return invalid_instruction;
	}
	
	/*
	If it's a remove instruction, the third element should also be either an integer or the keyword "end"
	*/
	if (is_remove_instruction) {
		third_element = to_lower(instruction_vector[2]);

		third_element_is_end = (third_element == "end");

		if ( !(is_number(instruction_vector[2]) || (third_element_is_end)) ) {
			invalid_instruction.set_error_message("Invalid REMOVE instruction: '" + instruction_vector[2] + "' is not a positive integer");
			return invalid_instruction;
		}
	}
	
	/*
	Now, to actually create the 'instruction' object
	*/

	int64_t start_position;
	int64_t end_position;

	if (is_replace_instruction) {
		if (second_element_is_end) {
			start_position = -1;
		} else {
			start_position = (int64_t)stoll(instruction_vector[1]);
		}
		
		return create_replace_instruction(start_position, instruction_vector[2]);
	}
	
	if (is_insert_instruction) {
		if (second_element_is_end) {
			start_position = -1;
		} else {
			start_position = (int64_t)stoll(instruction_vector[1]);
		}
		
		return create_insert_instruction(start_position, instruction_vector[2]);
	}
	
	if (is_remove_instruction) {
		if (second_element_is_end) {
			start_position = -1;
		} else {
			start_position = (int64_t)stoll(instruction_vector[1]);
		}
		
		if (third_element_is_end) {
			end_position = -1;
		} else {
			end_position = (int64_t)stoll(instruction_vector[2]);
		}
		
		return create_remove_instruction(start_position, end_position);
	}
	// Control shouldn't ever reach this part
	invalid_instruction.set_error_message("Control somehow reached the end of 'parse_instruction_string'. This shouldn't be possible. Please file a bug report");
	return invalid_instruction;
}


std::vector<editor::instruction> parse_instruction_sequence_string(std::string instruction_set_string) {
	/***
	std::vector<instruction> parse_instruction_sequence_string(std::string instruction_set_string):
		Create a std::vector of 'instruction' objects from a newline-delimited std::string of properly-formatted instructions
		
		Example of ONE properly-formatted "instruction set" string:
			REPLACE 5 hello
			INSERT 6 ni hao
			REMOVE 7 10
		
		Each line must follow the normal rules for properly-formatted instructions
			See: parse_instruction_string(std::string instruction_string)
		
		The "instruction set" itself must be newline-delimited
			That is, each individual instruction must be on its own line
	***/
	std::vector<editor::instruction> output_instruction_set;
	
	std::vector<std::string> instruction_strings = explode(instruction_set_string, '\n');
	
	for (std::string i : instruction_strings) {
		output_instruction_set.push_back(parse_instruction_string(i));
	}
	
	return output_instruction_set;
}
