/***
 * Copyright (C) 2024 rail5
*/

#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif

#ifndef SYS_FILE_H
	#define SYS_FILE_H
	#include <sys/file.h>
#endif

bool editor::file::set_file(std::string file_path) {
	/***
	bool set_file(std::string file_path):
		Initialize the file object
		
		Returns true if the file object is successfully initialized
		Only returns false if the directory that the file is meant to be in doesn't exist
	***/
	
	std::string directory = isolate_path_from_filename(file_path);
	
	file_name = file_path;
	file_directory = directory;
	
	/*
	Verify that the path is not a directory
	*/
	if (is_directory(file_path)) {
		error_message = file_name + " is a directory";
		initialized = false;
		return initialized;
	}

	/*
	Return error if the directory that the file is supposed to be in doesn't exist
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
		std::ofstream new_file(file_name);
		new_file.write("\n", 1);
		new_file.close();

		file_is_new = true;
		
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
	
	file_stream = std::fstream(file_name, std::ios::binary | std::ios::out | std::ios::in);
	
	/*
	Obtain file_descriptor in order to lock the file
	Unfortunately, the only portable way to do this is to open a SECOND file stream with the C-type FILE*
		rather than continuing with our luxury C++ std::fstream
	This FILE* will never be used, except to get the file descriptor so that we can lock the file during editing
	*/
	c_type_file = fopen64(file_name.c_str(), "a+");
	file_descriptor = fileno(c_type_file);
	
	if (flock(file_descriptor, LOCK_EX) == -1) {
		error_message = "Could not obtain file lock";
		initialized = false;
		return initialized;
	}
	
	file_length = std::filesystem::file_size(file_path);
	file_length_after_last_instruction = file_length;
	
	/*
	Set "initialized" to true if we've made it this far
	*/
	initialized = true;
	
	/*
	Clear any error message
	*/
	error_message = "";
	
	if (verbose) {
		std::cout << "Set file to " << file_path << std::endl;
	}
	
	/*
	Return likewise
	*/
	return initialized;
}

void editor::file::set_block_size(int64_t specified_blocksize) {
	/***
	bool set_block_size(int64_t specified_blocksize):
		Set the block size to inputted blocksize
			The "block size" is how much of the file (in bytes) we're willing to load into memory at one time
	***/
	
	block_size = specified_blocksize;
}

bool editor::file::add_instruction(instruction &input_instruction) {
	/***
	bool add_instruction(instruction input_instruction):
		Add an 'instruction' object to the instruction_sequence
			An 'instruction' object tells us what kind of edit to make
			For instance, "replace character #5 with a 'c'"
			Or "insert a 'q' at position #56"
			Or "remove character #3"
		
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
	Check for positions marked '-1.'
	Position '-1' is reserved for the "end" keyword, and is used to refer dynamically to the end of the file
	*/
	bool start_position_is_eof = (input_instruction.get_start_position() == -1);
	bool end_position_is_eof = (input_instruction.get_end_position() == -1);

	if (start_position_is_eof) {
		if (input_instruction.get_operation_type() == replace_operation) {
			/*
			REPLACE instructions are a special case with the 'END' keyword
				The user should be allowed to replace the last *few* characters of the file
					e.g.: "REPLACE END abcd"
			In which case we should calculate the start position as being, not EOF,
				but EOF - (text_input.length())
			*/
			input_instruction.update_start_position(
				file_length_after_last_instruction - input_instruction.get_text().length() - 1);
		} else {
			input_instruction.update_start_position(file_length_after_last_instruction - 1);

			if (input_instruction.get_operation_type() == remove_operation) {
				// For REMOVE instructions, should be file_length - 2, not - 1
				// Meaning: the "start position" pointer should point JUST BEFORE the character we want to act on
				input_instruction.update_start_position(input_instruction.get_start_position() - 1);
			}
		}

		input_instruction.update_end_position(input_instruction.get_start_position() + input_instruction.get_text().length());
	}

	if (end_position_is_eof) {
		// end_position_is_eof can never be set high unless we're running a REMOVE instruction
		input_instruction.update_end_position(file_length_after_last_instruction - 1);
	}
	
	/*
	Now, make sure that the start position isn't further than the end of the file, and isn't lower than zero
	*/
	if ( (input_instruction.get_start_position() >= file_length_after_last_instruction && file_length_after_last_instruction > 0) ||
		(input_instruction.get_start_position() > 0 && file_length_after_last_instruction == 0) ||
		(input_instruction.get_start_position() < 0) ) {
		input_instruction.set_error_message("Invalid start position", input_instruction.get_operation_type(), input_instruction.get_start_position(), input_instruction.get_end_position(), input_instruction.get_text());
		return false;
	}

	/*
	Next, make sure that the end position isn't further than the end of the file IF we're doing anything other than an insert (operation type #1, see namespace_editor.h for list of operation types)
	*/
	if (input_instruction.get_operation_type() != insert_operation) {
		if (input_instruction.get_end_position() >= file_length_after_last_instruction) {
			input_instruction.set_error_message("Invalid end position", input_instruction.get_operation_type(), input_instruction.get_start_position(), input_instruction.get_end_position(), input_instruction.get_text());
			return false;
		}
	}
	
	/*
	Add the instruction to the set, update file_length_after_last_instruction, and return true
	*/
	instruction_sequence.push_back(input_instruction);
	
	if (input_instruction.get_operation_type() == insert_operation) {
		file_length_after_last_instruction = (file_length_after_last_instruction + input_instruction.get_text().length());
	} else if (input_instruction.get_operation_type() == remove_operation) {
		int64_t remove_length = (input_instruction.get_end_position() - input_instruction.get_start_position());
		file_length_after_last_instruction = (file_length_after_last_instruction - remove_length);
	}
	
	return true;
}

bool editor::file::is_initialized() {
	return initialized;
}

std::string editor::file::get_file_name() {
	return file_name;
}

std::string editor::file::get_file_directory() {
	return file_directory;
}

int64_t editor::file::get_block_size() {
	return block_size;
}

int64_t editor::file::get_file_length() {
	return file_length;
}

std::vector<editor::instruction> editor::file::get_instruction_sequence() {
	return instruction_sequence;
}

std::string editor::file::get_error_message() {
	return error_message;
}

void editor::file::replace(int64_t start_position, std::string replacement_text) {
	/***
	void editor::file::replace(int64_t start_position, std::string replacement_text):
		Execute a "REPLACE" instruction
		Opens a file stream & replaces text inside the file, starting from start_position, with replacement_text
	***/

	if (!file_stream.is_open()) {
		return;
	}
	
	// Seek to start_position
	file_stream.seekp(start_position, std::ios::beg);
	// Replace
	file_stream.write(replacement_text.c_str(), replacement_text.length());
	
	// Flush changes
	file_stream.flush();
	fflush(c_type_file);
	
	if (verbose) {
		std::cout << "Executed REPLACE instruction (" << start_position << ", " << replacement_text << ")" << std::endl;
	}
}

void editor::file::insert(int64_t start_position, std::string text_to_insert) {
	/***
	void editor::file::insert(int64_t start_position, std::string text_to_insert):
		Execute an "INSERT" instruction
		Opens a file stream & inserts text_to_insert into the file at position start_position, without replacing
	***/
	
	if (!file_stream.is_open()) {
		return;
	}

	int64_t insert_length = text_to_insert.length();
		
	int64_t new_file_length = file_length + insert_length;
		
	bool writing_to_eof = (start_position == (file_length - 1));
	
	// Are we writing to EOF, or before EOF?
	if (writing_to_eof) {
		// Writing TO EOF
		
		// Seek to EOF
		file_stream.seekp(start_position, std::ios::beg);
		
		// Insert
		file_stream.write(text_to_insert.c_str(), text_to_insert.length());
		
		// Add a newline char
		file_stream.seekp(new_file_length - 1, std::ios::beg);
		file_stream.write("\n", 1);
		
		// Flush changes
		file_stream.flush();
		fflush(c_type_file);
		
		// Update file_length for future instructions
		file_length = new_file_length;
		
		if (verbose) {
			std::cout << "Executed INSERT instruction (" << start_position << ", " << text_to_insert << ")" << std::endl;
		}
		
		return;
	}
	
	// Writing BEFORE EOF
	
	int64_t amount_to_store = block_size;
	
	// Adjust the length of the file by adding 0s to the end
	ftruncate(file_descriptor, new_file_length);
	
	if (verbose) {
		std::cout << "Adjusted file length to " << new_file_length << std::endl;
	}
	
	// Add a newline char
	file_stream.seekp(new_file_length - 1, std::ios::beg);
	file_stream.write("\n", 1);
	bool force_add_newline = false;
	
	for (int64_t i = (new_file_length - 1); i > start_position; i = (i - amount_to_store)) {
		int64_t copy_to_this_position = i - amount_to_store;
		int64_t copy_from_this_position = (copy_to_this_position - insert_length);

		// Final iteration:
		// If we discover that our "copy_from" position is before our start_position,
			// (Due to block_size shenanigans --
			// E.g: block_size = 1024, but we only have 512 bytes left in the loop)
		// Then we re-set the copy_from to the start_position,
		// Fix the copy_to, change amount_to_store to what's LEFT
			// (In the above example, 512 rather than 1024)
		// And set i = start_position to make sure the loop doesn't run again after this
		if (copy_from_this_position < start_position) {
			int64_t difference = start_position - copy_from_this_position;
			
			copy_from_this_position = start_position;
			
			copy_to_this_position = copy_to_this_position + difference;

			amount_to_store = amount_to_store - difference;
			
			i = start_position; // Terminate loop
		}

		// HACKY fix for a bug inserting to files of 0 size
		// Total rewrite of both editor & reader incoming very very soon
		if (amount_to_store < 0) {
			amount_to_store = 0;
			force_add_newline = true;
		}
		
		char* temp_data_storage = new char[amount_to_store + 1]{0}; // Allocate memory
		
		// Store read portion into allocated memory
		file_stream.seekg(copy_from_this_position, std::ios::beg);
		file_stream.read(temp_data_storage, amount_to_store);
		
		// Add a NUL char to the end to terminate the string
		temp_data_storage[amount_to_store] = 0;
		
		// Copy it to its new proper place
		file_stream.seekp(copy_to_this_position, std::ios::beg);
		file_stream.write(temp_data_storage, amount_to_store);
		
		delete[] temp_data_storage; // Free memory

		if (verbose) {
			std::cout << "Moved " << amount_to_store << " bytes to position #" << copy_to_this_position << " for INSERT instruction" << std::endl;
		}
	}
	
	// Now, finally, insert the damn data (user inputted data)
	file_stream.seekp(start_position, std::ios::beg);
	file_stream.write(text_to_insert.c_str(), text_to_insert.length());

	// If we have to force add a newline, do it now
	if (force_add_newline) {
		file_stream.seekp(new_file_length, std::ios::beg);
		file_stream.write("\n", 1);
		force_add_newline = false; // Reset
	}
		
	
	// Flush changes
	file_stream.flush();
	fflush(c_type_file);
	
	// Update file_length for future instructions
	file_length = new_file_length;
	
	if (verbose) {
		std::cout << "Executed INSERT instruction (" << start_position << ", " << text_to_insert << ")" << std::endl;
	}
}

void editor::file::remove(int64_t start_position, int64_t end_position) {
	/***
	void editor::file::remove(int64_t start_position, int64_t end_position):
		Execute a "REMOVE" instruction
		Opens a file stream & removes text from start_position to end_position,
			shifting everything after end_position to the left
	***/
	
	if (!file_stream.is_open()) {
		return;
	}
	
	int64_t remove_length = (end_position - start_position);
	
	int64_t new_file_length = (file_length - remove_length);
	
	int64_t amount_to_store = block_size;
	
	// Are we deleting the end of the file, or something before the end of the file?
	if (end_position == (file_length - 1)) {
		// EOF
		
		// Truncate to new_file_length
		ftruncate(file_descriptor, new_file_length);
		
		// Add a newline char
		file_stream.seekp(new_file_length - 1, std::ios::beg);
		file_stream.write("\n", 1);
		
		// Flush changes
		file_stream.flush();
		fflush(c_type_file);
		
		// Update file_length for future instructions
		file_length = new_file_length;
		
		if (verbose) {
			std::cout << "Executed REMOVE instruction (" << start_position << ", " << end_position << ")" << std::endl;
		}
		
		return;
	}
	
	// Before EOF
	for (int64_t i = start_position; i < (new_file_length - 1); i = (i + amount_to_store)) {
		int64_t copy_to_this_position = i;
		
		int64_t copy_from_this_position = (i + remove_length);
		
		// Final iteration:
		// If we discover that our block_size is more than all the data that's left,
		// we just reset it to the amount of data that's left.
		if ((copy_from_this_position + amount_to_store) > (file_length - 1)) {
			amount_to_store = ((file_length - 1) - copy_from_this_position);
		}
		
		char* temp_data_storage = new char[amount_to_store + 1]{0}; // Allocate memory
		
		// Store read portion into allocated memory
		file_stream.seekg(copy_from_this_position, std::ios::beg);
		file_stream.read(temp_data_storage, amount_to_store);
		
		// Add a NUL char to the end to terminate the string
		temp_data_storage[amount_to_store] = 0;
		
		// Copy it to its new proper place
		file_stream.seekp(copy_to_this_position, std::ios::beg);
		file_stream.write(temp_data_storage, amount_to_store);
		
		delete[] temp_data_storage; // Free memory
		
		if (verbose) {
			std::cout << "Moved " << amount_to_store << " bytes to position #" << copy_to_this_position << " for REMOVE instruction" << std::endl;
		}
	}
	
	// Now, finally, truncate the file to its new length
	ftruncate(file_descriptor, new_file_length);
	
	// Add a newline char
	file_stream.seekp(new_file_length - 1, std::ios::beg);
	file_stream.write("\n", 1);
	
	// Flush changes
	file_stream.flush();
	fflush(c_type_file);
	
	// Update file_length for future instructions
	file_length = new_file_length;
	
	if (verbose) {
		std::cout << "Executed REMOVE instruction (" << start_position << ", " << end_position << ")" << std::endl;
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
	
	if (instruction_to_execute.get_operation_type() == remove_operation) {
		remove(instruction_to_execute.get_start_position(), instruction_to_execute.get_end_position());
		return true;
	}
	
	return false;
}

void editor::file::close() {
	/* Close the std::fstream */
	file_stream.close();
	/* Close the C-type FILE* */
	fclose(c_type_file);
	/* Unlock the file */
	flock(file_descriptor, LOCK_UN);
}

void editor::file::clean_up() {
	/***
	 * The point of this function is to _delete_ the file in the specific case that BOTH of the following are true:
	 * 	1. We created it (it is a new file, didn't exist before we started here)
	 * 	2. We had to exit early due to some error
	 ***/
	
	if (file_is_new) {
		std::remove(file_name.c_str());
	}
}


editor::file::file(std::string path, int64_t blocksize, bool verbose_mode) {
	block_size = blocksize;
	verbose = verbose_mode;
	set_file(path);
}
