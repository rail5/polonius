#ifndef FSTREAM
	#define FSTREAM
	#include <fstream>
#endif

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

using namespace std;

bool reader::file::init(string path) {
	if (!file_exists(path)) {
		init_error_message = "File '" + path + "' does not exist";
		return false;
	}
	
	file_name = path;
	
	ifstream file_stream(file_name, ifstream::binary);
	
	if (!file_stream) {
		init_error_message = "Could not open file '" + file_name + "' for reading";
		return false;
	}
	
	// get length of file
	file_stream.seekg(0, file_stream.end);
	file_length = file_stream.tellg();
	
	initialized = true;
	
	return true;
}

string reader::file::read(int64_t start_position, int64_t length) {

	ifstream file_stream(file_name, ifstream::binary);
	
	// allocate memory
	string buffer(length, ' ');
	
	// set position
	file_stream.seekg(start_position);
	
	// read data as a block
	file_stream.read(&buffer[0], length);
	
	file_stream.close();
	
	return buffer;
}

string reader::file::search(string query) {
	
}

void reader::file::do_job() {
	if (!initialized) {
		cout << "Error reading file" << endl;
		return;
	}
	
	// Make sure we're not trying to read outside the bounds of the file
	// And fail open if so
	
	// First check the start position
	if (start_position > file_length) {
		// And just read the last byte
		start_position = (file_length - 2);
	}
	
	// Now check the end position
	int64_t end_position = (start_position + amount_to_read);
	
	if (end_position > file_length) {
		// Just set it to read till the end of the file & not further
		amount_to_read = (file_length - start_position);
		end_position = (start_position + amount_to_read);
	}

	if (job == read_job) {
		/*
		If just_outputting_positions == true,
		Then we don't need to actually read the file,
		Just output in the format startposition,endposition
		*/
		if (just_outputting_positions) {
			cout << start_position << "," << end_position;
			return;
		}
		
		for (int64_t i = start_position; i <= end_position; (i = i + block_size)) {
			int64_t amount_left_in_file = (end_position - i);
			
			if (block_size > amount_left_in_file) {
				block_size = amount_left_in_file;
			}
			
			if (block_size == 0) {
				return; // Terminate loop
			}
			
			cout << read(i, block_size);
		}
		return;
	}
}

string reader::file::get_init_error_message() {
	return init_error_message;
}

int64_t reader::file::get_file_length() {
	return file_length;
}

void reader::file::set_start_position(int64_t position) {
	start_position = position;
}

void reader::file::set_amount_to_read(int64_t amount) {
	amount_to_read = amount;
}

void reader::file::set_just_outputting_positions(bool flag) {
	just_outputting_positions = flag;
}

void reader::file::set_block_size(int size) {
	block_size = size;
}

void reader::file::set_job_type(job_type input_job) {
	job = input_job;
}
