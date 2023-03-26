#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif

#ifndef SYS_FILE_H
	#define SYS_FILE_H
	#include <sys/file.h>
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
	Return error if the directory doesn't exist
	*/
	if (!file_exists(directory)) {
		error_message = "Path '" + directory + "' does not exist";
		initialized = false;
		return initialized;
	}
	
	/*
	Create the file if it doesn't already exist
	*/
	if (!file_exists(file_name)) {
		ofstream new_file(file_name);
		new_file.close();
		
		/*
		If the file still doesn't exist, we don't have write permissions to the directory
		*/
		if (!file_exists(file_name)) {
			error_message = "File could not be created";
			initialized = false;
			return initialized;
		}
		
	}
	
	/*
	Check if we have write permissions to the file
	*/
	if (!file_is_writable(file_name)) {
		error_message = "File is not writable by current user";
		initialized = false;
		return initialized;
	}
	
	/*
	Initialize the file stream, set a POSIX file lock, and set file_length
	*/
	
	file_stream = fstream(file_name, ios::binary | ios::out | ios::in);
	
	/*
	Obtain file_descriptor in order to lock the file
	Unfortunately, the only portable way to do this is to open a SECOND file stream with the C-type FILE*
		rather than continuing with our luxury C++ fstream
	This FILE* will never be used, except to get the file descriptor so that we can lock the file during editing
	*/
	c_type_file = fopen(file_name.c_str(), "a+");
	file_descriptor = fileno(c_type_file);
	
	if (flock(file_descriptor, LOCK_EX) == -1) {
		error_message = "Could not obtain file lock";
		initialized = false;
		return initialized;
	}
	
	file_length = filesystem::file_size(file_path);
	
	/*
	Set "initialized" to true if we've made it this far
	*/
	initialized = true;
	
	/*
	Clear any error message
	*/
	error_message = "";
	
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
	if (input_instruction.get_start_position() >= file_length) {
		input_instruction.set_error_message("Invalid start position");
		return false;
	}

	/*
	Next, make sure that the end position isn't further than the end of the file IF we're doing anything other than an insert (operation type #1, see namespace_editor.h for list of operation types)
	*/
	if (input_instruction.get_operation_type() != insert_operation) {
		if (input_instruction.get_end_position() >= file_length) {
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

long long int editor::file::get_file_length() {
	return file_length;
}

vector<editor::instruction> editor::file::get_instruction_set() {
	return instruction_set;
}

string editor::file::get_error_message() {
	return error_message;
}

void editor::file::replace(long long int start_position, string replacement_text) {
	/***
	void editor::file::replace(long long int start_position, string replacement_text):
		Execute a "REPLACE" instruction
		Opens a file stream & replaces text inside the file, starting from start_position, with replacement_text
	***/

	if (!file_stream.is_open()) {
		return;
	}
	
	// Seek to start_position
	file_stream.seekp(start_position, ios::beg);
	// Replace
	file_stream.write(replacement_text.c_str(), replacement_text.length());
}

void editor::file::insert(long long int start_position, string text_to_insert) {
	/***
	void editor::file::insert(long long int start_position, string text_to_insert):
		Execute an "INSERT" instruction
		Opens a file stream & inserts text_to_insert into the file at position start_position, without replacing
	***/
	
	if (!file_stream.is_open()) {
		return;
	}

	long long int insert_length = text_to_insert.length();
		
	long long int new_file_length = file_length + insert_length;
	
	bool creating_new_file = (start_position == 0 && file_length == 0);
	
	bool writing_to_eof = (start_position == (file_length - 1));
	
	// Are we writing to EOF, or before EOF?
	if (writing_to_eof || creating_new_file) {
		// Writing TO EOF
		
		// Seek to EOF
		file_stream.seekp(start_position, ios::beg);
		
		// Insert
		file_stream.write(text_to_insert.c_str(), text_to_insert.length());
		
		if (creating_new_file) {
			// Compensate for the fact that we have to add a newline char to the end
			new_file_length = new_file_length + 1;
		}
		
		// Add a newline char
		file_stream.seekp(new_file_length - 1, ios::beg);
		file_stream.write("\n", 1);
		
		return;
	}
	
	// Writing BEFORE EOF
	
	int amount_to_store = block_size;
	
	// Adjust the length of the file by adding 0s to the end
	ftruncate(file_descriptor, new_file_length);
	
	// Add a newline char
	file_stream.seekp(new_file_length - 1, ios::beg);
	file_stream.write("\n", 1);
	
	for (long long int i = (new_file_length - 1); i > start_position; i = (i - amount_to_store)) {
	
		long long int copy_to_this_position = (i - (amount_to_store - 1)) - 1;
		long long int copy_from_this_position = (copy_to_this_position - insert_length);

		// Final iteration:
		// If we discover that our "copy_from" position is before our start_position,
			// (Due to block_size shenanigans --
			// E.g: block_size = 1024, but we only have 512 bytes left in the loop)
		// Then we re-set the copy_from to the start_position,
		// Fix the copy_to, change amount_to_store to what's LEFT
			// (In the above example, 512 rather than 1024)
		// And set i = start_position to make sure the loop doesn't run again after this
		if (copy_from_this_position < start_position) {
		
			long long int difference = start_position - copy_from_this_position;
			
			copy_from_this_position = start_position;
			
			copy_to_this_position = copy_to_this_position + difference;
			
			amount_to_store = amount_to_store - difference;
			
			i = start_position; // Terminate loop
		}
		
		char* temp_data_storage = new char[amount_to_store + 1]{0}; // Allocate memory
		
		// Store read portion into allocated memory
		file_stream.seekg(copy_from_this_position, ios::beg);
		file_stream.read(temp_data_storage, amount_to_store);
		
		// Add a NUL char to the end to terminate the string
		temp_data_storage[amount_to_store] = 0;
		
		// Copy it to its new proper place
		file_stream.seekp(copy_to_this_position, ios::beg);
		file_stream.write(temp_data_storage, amount_to_store);
		
		delete[] temp_data_storage; // Free memory
	}
	
	// Now, finally, insert the damn data (user inputted data)
	file_stream.seekp(start_position, ios::beg);
	file_stream.write(text_to_insert.c_str(), text_to_insert.length());
}

void editor::file::remove(long long int start_position, long long int end_position) {
	/***
	void editor::file::remove(long long int start_position, long long int end_position):
		Execute a "REMOVE" instruction
		Opens a file stream & removes text from start_position to end_position,
			shifting everything after end_position to the left
	***/
	
	if (!file_stream.is_open()) {
		return;
	}
	
	long long int remove_length = (end_position - start_position);
	
	long long int new_file_length = (file_length - remove_length);
	
	int amount_to_store = block_size;
	
	// Are we deleting the end of the file, or something before the end of the file?
	if (end_position == (file_length - 1)) {
		// EOF
		
		// Truncate to new_file_length
		ftruncate(file_descriptor, new_file_length);
		
		// Add a newline char
		file_stream.seekp(new_file_length - 1, ios::beg);
		file_stream.write("\n", 1);
		
		return;
	}
	
	// Before EOF
	for (long long int i = start_position; i < (new_file_length - 1); i = (i + amount_to_store)) {
		
		long long int copy_to_this_position = i;
		
		long long int copy_from_this_position = (i + remove_length);
		
		// Final iteration:
		// If we discover that our block_size is more than all the data that's left,
		// we just reset it to the amount of data that's left.
		if ((copy_from_this_position + amount_to_store) > (file_length - 1)) {
			amount_to_store = ((file_length - 1) - copy_from_this_position);
		}
		
		char* temp_data_storage = new char[amount_to_store + 1]{0}; // Allocate memory
		
		// Store read portion into allocated memory
		file_stream.seekg(copy_from_this_position, ios::beg);
		file_stream.read(temp_data_storage, amount_to_store);
		
		// Add a NUL char to the end to terminate the string
		temp_data_storage[amount_to_store] = 0;
		
		// Copy it to its new proper place
		file_stream.seekp(copy_to_this_position, ios::beg);
		file_stream.write(temp_data_storage, amount_to_store);
		
		delete[] temp_data_storage; // Free memory
	}
	
	// Now, finally, truncate the file to its new length
	ftruncate(file_descriptor, new_file_length);
	
	// Add a newline char
	file_stream.seekp(new_file_length - 1, ios::beg);
	file_stream.write("\n", 1);
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
	
	if (instruction_to_execute.get_operation_type() == remove_operation) {
		remove(instruction_to_execute.get_start_position(), instruction_to_execute.get_end_position());
		return true;
	}
	
	return false;
}

editor::file add_file(string file_path) {
	editor::file new_file;
	
	new_file.set_file(file_path);
	
	return new_file;
}

void editor::file::close() {
	/* Close the fstream */
	file_stream.close();
	/* Close the C-type FILE* */
	fclose(c_type_file);
	/* Unlock the file */
	flock(file_descriptor, LOCK_UN);
}
