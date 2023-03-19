#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif

using namespace std;

bool editor::file::set_file(string file_path) {
	/***
	bool set_file(string file_path):
		Initialize the file object
		
		Returns true if the file object is successfully initialized
		Only returns false if the directory that the file is meant to be in doesn't exist
	***/
	
	/*
	Verify that the directory that the file is supposed to be in actually exists
	*/
	string directory = isolate_path_from_filename(file_path);
	
	file_name = file_path;
	file_directory = directory;
	
	/*
	If we're not creating a new file, set file_length
	*/
	if (file_exists(file_name)) {
		file_length = filesystem::file_size(file_path);
	}
	
	/*
	Set "initialized" to true if the directory exists, false if not
	*/
	initialized = file_exists(directory);
	
	/*
	Return likewise
	*/
	return initialized;
}

void editor::file::set_block_size(int specified_blocksize) {
	/***
	bool set_block_size(int specified_blocksize):
		Set the block size to inputted blocksize
			The "block size" is how much of the file (in bytes) we're willing to load into memory at one time
	***/
	
	block_size = specified_blocksize;
}

bool editor::file::add_instruction(instruction &input_instruction) {
	/***
	bool add_instruction(instruction input_instruction):
		Add an 'instruction' object to the instruction_set
			An 'instruction' object tells us what kind of edit to make
			For instance, "replace byte #5 with a 'c'"
			Or "insert a 'q' at position #56"
			Or "remove byte #3"
		
		Returns true if the instruction was successfully added
		Returns false if the file is uninitialized
		Returns false & sets the instruction's error message
			if the instruction cannot be added
	***/
	
	/*
	Are we initialized?
	*/
	
	if (!initialized) {
		return false;
	}
	
	/*
	Verify the instruction is sane
	First, make sure the instruction is initialized
	*/
	if (!input_instruction.is_initialized()) {
		return false;
	}
	
	/*
	Now, make sure that the start position isn't further than the end of the file
	*/
	if (input_instruction.get_start_position() > file_length) {
		input_instruction.set_error_message("Invalid start position");
		return false;
	}

	/*
	Next, make sure that the end position isn't further than the end of the file IF we're doing anything other than an insert (operation type #1, see namespace_editor.h for list of operation types)
	*/
	if (input_instruction.get_operation_type() != insert_operation) {
		if (input_instruction.get_end_position() > file_length) {
			input_instruction.set_error_message("Invalid end position");
			return false;
		}
	}
	
	/*
	Add the instruction to the set and return true
	*/
	instruction_set.push_back(input_instruction);
	
	return true;
}

bool editor::file::is_initialized() {
	return initialized;
}

string editor::file::get_file_name() {
	return file_name;
}

string editor::file::get_file_directory() {
	return file_directory;
}

int editor::file::get_block_size() {
	return block_size;
}

int editor::file::get_file_length() {
	return file_length;
}

vector<editor::instruction> editor::file::get_instruction_set() {
	return instruction_set;
}

void editor::file::replace(int start_position, string replacement_text) {
	/***
	void editor::file::replace(int start_position, string replacement_text):
		Execute a "REPLACE" instruction
		Opens a file stream & replaces text inside the file, starting from start_position, with replacement_text
	***/
	
	// Open the file stream
	fstream edit_file(file_name, ios::binary | ios::out | ios::in);
	
	if (edit_file.is_open()) {
		// Seek to start_position
		edit_file.seekp(start_position, ios::beg);
		// Replace
		edit_file.write(replacement_text.c_str(), replacement_text.length());
	}
}

void editor::file::insert(int start_position, string text_to_insert) {
	/***
	void editor::file::insert(int start_position, string text_to_insert):
		Execute an "INSERT" instruction
		Opens a file stream & inserts text_to_insert into the file at position start_position, without replacing
	***/
	
	// Open the file stream
	fstream edit_file(file_name, ios::binary | ios::out | ios::in);
	
	if (edit_file.is_open()) {
	
		int insert_length = text_to_insert.length();
			
		int new_file_length = file_length + insert_length;
		
		// Are we writing to EOF, or in the middle of the file?
		if (start_position == (file_length - 1)) {
			// Writing TO EOF
			
			// Seek to EOF
			edit_file.seekp(start_position, ios::beg);
			
			// Insert
			edit_file.write(text_to_insert.c_str(), text_to_insert.length());
			
			// Add a newline char
			edit_file.seekp(new_file_length - 1, ios::beg);
			edit_file.write("\n", 1);
		} else {
			// Writing BEFORE EOF
			
			int amount_to_store = block_size;
			
			// Adjust the length of the file by adding 0s to the end
			for (int i = (file_length - 1); i < (new_file_length - 1); i++) {
				edit_file.seekp(i, ios::beg);
				edit_file.write("0", 1);
			}
			
			// Add a newline char
			edit_file.seekp(new_file_length - 1, ios::beg);
			edit_file.write("\n", 1);
			
			for (int i = (new_file_length - 1); i > start_position; i = (i - amount_to_store)) {
				
				int copy_to_this_position = (i - (amount_to_store - 1)) - 1;
				int copy_from_this_position = (copy_to_this_position - insert_length);
				
				// Final iteration:
				// If we discover that our "copy_from" position is before our start_position,
					// (Due to block_size shenanigans --
					// E.g: block_size = 1024, but we only have 512 bytes left in the loop)
				// Then we re-set the copy_from to the start_position,
				// Fix the copy_to, change amount_to_store to what's LEFT
					// (In the above example, 512 rather than 1024)
				// And set i = start_position to make sure the loop doesn't run again after this
				if (copy_from_this_position < start_position) {
				
					int difference = start_position - copy_from_this_position;
					
					copy_from_this_position = start_position;
					
					copy_to_this_position = copy_to_this_position + difference;
					
					amount_to_store = amount_to_store - difference;
					
					i = start_position; // Terminate loop
				}
				
				char* temp_data_storage = new char[amount_to_store + 1]{0}; // Allocate memory
				
				// Store read portion into allocated memory
				edit_file.seekg(copy_from_this_position, ios::beg);
				edit_file.read(temp_data_storage, amount_to_store);
				
				// Add a NUL char to the end to terminate the string
				temp_data_storage[amount_to_store] = 0;
				
				// Copy it to its new proper place
				edit_file.seekp(copy_to_this_position, ios::beg);
				edit_file.write(temp_data_storage, amount_to_store);
				
				delete[] temp_data_storage; // Free memory
			}
			
			// Now, finally, insert the damn data (user inputted data)
			edit_file.seekp(start_position, ios::beg);
			edit_file.write(text_to_insert.c_str(), text_to_insert.length());
		}
	}
}

bool editor::file::execute_single_instruction(instruction instruction_to_execute) {

	if (instruction_to_execute.get_operation_type() == replace_operation) {
		replace(instruction_to_execute.get_start_position(), instruction_to_execute.get_text());
		return true;
	}
	
	if (instruction_to_execute.get_operation_type() == insert_operation) {
		insert(instruction_to_execute.get_start_position(), instruction_to_execute.get_text());
		return true;
	}
	
	return false;
}

editor::file add_file(string file_path) {
	editor::file new_file;
	
	new_file.set_file(file_path);
	
	return new_file;
}
