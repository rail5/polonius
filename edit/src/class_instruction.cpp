using namespace std;

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

bool editor::instruction::set_replace_instruction(int start, string text) {
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
		- Not less than zero
	*/
	if (start < 0) {
		error_message = "Invalid start position";
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

bool editor::instruction::set_insert_instruction(int start, string text) {
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
		- Not less than zero
	*/
	if (start < 0) {
		error_message = "Invalid start position";
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

bool editor::instruction::set_remove_instruction(int start, int end) {
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
		- Not less than zero
	*/
	if (start < 0) {
		error_message = "Invalid start position";
		clear_instruction();
		return false;
	}
	
	/*
	Make sure the end position is valid
	Meaning:
		- Not less than  the start position
	*/
	if (end < start) {
		error_message = "Invalid end position";
		clear_instruction();
		return false;
	}
	
	/*
	Set object 'private' info to user-provided parameters
	And then declare the object initialized
	*/
	operation = remove_operation;
	start_position = start;
	end_position = end;
	initialized = true;
	
	/* Clear any earlier error messages */
	error_message = "";
	
	return true;
}

void editor::instruction::set_error_message(string message) {
	/***
	void set_error_message(string message):
		Set the instruction's "error_message" to the inputted string
	***/
	
	error_message = message;
}

string editor::instruction::get_error_message() {
	return error_message;
}

bool editor::instruction::is_initialized() {
	return initialized;
}

int editor::instruction::get_operation_type() {
	return operation;
}

int editor::instruction::get_start_position() {
	return start_position;
}

int editor::instruction::get_end_position() {
	return end_position;
}

string editor::instruction::get_text() {
	return text_input;
}

editor::instruction create_replace_instruction(int start_position, string text) {
	editor::instruction new_instruction;
	
	new_instruction.set_replace_instruction(start_position, text);
	
	return new_instruction;
}

editor::instruction create_insert_instruction(int start_position, string text) {
	editor::instruction new_instruction;
	
	new_instruction.set_insert_instruction(start_position, text);
	
	return new_instruction;
}

editor::instruction create_remove_instruction(int start_position, int end_position) {
	editor::instruction new_instruction;
	
	new_instruction.set_remove_instruction(start_position, end_position);
	
	return new_instruction;
}


editor::instruction parse_instruction_string(string instruction_string) {
	/*
	instruction parse_instruction_string(string instruction_string):
		Create an 'instruction' object from a properly-formatted string
		
		The string must be formatted in one of the following ways:
			1.
				REPLACE 5 "hello"
					(
					This would replace bytes #5 - #9 with "hello"
						Example:
							Original file:
								01234567890123
							New file:
								01234hello0123
					)
			2.
				INSERT 6 "ni hao"
					(
					This would shift bytes #6 - EOF rightward and insert "ni hao" at position #6 without replacing
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
					This would remove bytes #7 - #10 and shift the remaining bytes leftward
						Example:
							Original file:
								01234567890123
							New file:
								0123456123
					)
		That is, the string must be formatted with these rules:
			- Space-delimited
			- The first key is the instruction name (REPLACE, INSERT, or REMOVE)
			- The second key is the start position
			- The third key is either:
				- A quote-escaped string (in the case of REPLACE or INSERT)
					[Can be either double-quotes or single-quotes]
				- The end position (in the case of REMOVE)
	*/
	
	/*
	First, split the string into a vector<string> delimited by spaces
		See: shared_functions/explode.cpp
	*/
	vector<string> instruction_vector = explode(instruction_string, ' ', 3);
	
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
	
	/*
	Make sure the first element is either "replace," "insert," or "remove"
	*/
	string first_element = to_lower(instruction_vector[0]);
	bool is_replace_instruction = (first_element == "replace");
	bool is_insert_instruction = (first_element == "insert");
	bool is_remove_instruction = (first_element == "remove");
	
	bool instruction_type_is_valid = (is_replace_instruction || is_insert_instruction || is_remove_instruction);
	
	if (!instruction_type_is_valid) {
		invalid_instruction.set_error_message("Invalid instruction type: '" + instruction_vector[0] + "'");
		return invalid_instruction;
	}
	
	/*
	The second element should ALWAYS be an integer
		See: shared_functions/is_number.cpp
	*/
	if (!is_number(instruction_vector[1])) {
		invalid_instruction.set_error_message("Invalid instruction: '" + instruction_vector[1] + "' is not a positive integer");
		return invalid_instruction;
	}
	
	/*
	In a replace or insert instruction, the third element should be a quote-encased string
	In a remove instruction, it should be an integer
	*/
	if (is_replace_instruction || is_insert_instruction) {
		// Check that there's actually data to "insert" or "replace" with
		// The quote-marks will take up 2 characters, so there should be at least 3
		if (!(instruction_vector[2].size() > 2)) {
			invalid_instruction.set_error_message("Invalid instruction: No argument given");
			return invalid_instruction;
		}
	
		// Check that the first and last chars are matching quote-marks
		bool double_quotes_encased = ((instruction_vector[2][0] == '"') && (instruction_vector[2][instruction_vector[2].size()-1] == '"'));
		bool single_quotes_encased = ((instruction_vector[2][0] == '\'') && (instruction_vector[2][instruction_vector[2].size()-1] == '\''));
		
		bool encased_in_quotes = (double_quotes_encased || single_quotes_encased);
		
		if (!encased_in_quotes) {
			invalid_instruction.set_error_message("Invalid instruction: Argument is not encased in quotes");
			return invalid_instruction;
		}
		
	} else if (is_remove_instruction) {
		if (!is_number(instruction_vector[2])) {
			invalid_instruction.set_error_message("Invalid REMOVE instruction: '" + instruction_vector[2] + "' is not a positive integer");
			return invalid_instruction;
		}
	}
	
	/*
	Now, to actually create the 'instruction' object
	*/
	if (is_replace_instruction) {
		int start_position = (int)stol(instruction_vector[1]);
		
		// Remove the quote-marks from the text input (first and last chars)
		string text_input = instruction_vector[2].substr(1, instruction_vector[2].size()-2);
		
		return create_replace_instruction(start_position, text_input);
	}
	
	if (is_insert_instruction) {
		int start_position = (int)stol(instruction_vector[1]);

		// Remove the quote-marks from the text input (first and last chars)
		string text_input = instruction_vector[2].substr(1, instruction_vector[2].size()-2);
		
		return create_insert_instruction(start_position, text_input);
	}
	
	if (is_remove_instruction) {
		int start_position = (int)stol(instruction_vector[1]);
		
		int end_position = (int)stol(instruction_vector[2]);
		
		return create_remove_instruction(start_position, end_position);
	}
	
	// If we've made it this far, something terrible has happened
	invalid_instruction.set_error_message("I must suck at programming");
	return invalid_instruction;
}


vector<editor::instruction> parse_instruction_set_string(string instruction_set_string) {
	/***
	vector<instruction> parse_instruction_set_string(string instruction_set_string):
		Create a vector of 'instruction' objects from a newline-delimited string of properly-formatted instructions
		
		Example of ONE properly-formatted "instruction set" string:
			REPLACE 5 "hello"
			INSERT 6 "ni hao"
			REMOVE 7 10
		
		Each line must follow the normal rules for properly-formatted instructions
			See: parse_instruction_string(string instruction_string)
		
		The "instruction set" itself must be newline-delimited
			That is, each individual instruction must be on its own line
	***/
	vector<editor::instruction> output_instruction_set;
	
	vector<string> instruction_strings = explode(instruction_set_string, '\n');
	
	for (string i : instruction_strings) {
		output_instruction_set.push_back( parse_instruction_string(i) );
	}
	
	return output_instruction_set;
}
