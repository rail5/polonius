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

bool reader::file::do_read_job() {
	/*
	If just_outputting_positions == true,
	Then we don't need to actually read the file,
	Just output in the format "startposition endposition"
	*/
	if (just_outputting_positions) {
		cout << start_position << " " << end_position-1 << endl;
		return true;
	}
	
	for (int64_t i = start_position; i < end_position; (i = i + block_size)) {
		int64_t amount_left_in_file = (end_position - i);
		
		if (block_size > amount_left_in_file) {
			block_size = amount_left_in_file;
		}
		
		cout << read(i, block_size);
	}
	return true;
}

bool reader::file::do_normal_search() {
	int64_t match_start = 0;
	int64_t match_end = 0;
	
	// Only call the .length() function ONCE and store it
	// To save on resources
	int search_query_length = search_query.length();
	
	if (block_size <= search_query_length) {
		// If the block size is smaller than the length of the search query,
		// The user is just being silly.
		// We'll reset it to 1 larger than the length of the search query
		block_size = search_query_length + 1;
	}
	
	int shift_by_this_much = (block_size - search_query_length);
		
	for (int64_t i = start_position; i < end_position; (i = i + shift_by_this_much)) {
		int64_t amount_left_in_file = (end_position - i);
		
		if (block_size > amount_left_in_file || shift_by_this_much > amount_left_in_file) {
			block_size = amount_left_in_file;
			shift_by_this_much = amount_left_in_file;
		}
		
		string block_data = read(i, block_size);
		
		// Check if the WHOLE search query is in the block
		// And if so, just output it
		size_t search_result = block_data.find(search_query);
		
		// Next cycle if no match was found
		if (search_result == -1) {
			continue;
		}
			// Start & end position RELATIVE to this block
			start_position = search_result;
			end_position = (start_position + search_query_length);
			
			// ABSOLUTE start & end position
			// (that is, relative to the start of the file)
			match_start = (i + start_position);
			match_end = (match_start + search_query_length);
			
			if (just_outputting_positions) {
				cout << match_start << " " << match_end-1 << endl;
				return true;
			}
			
			cout << block_data.substr(start_position, search_query_length) << endl;
			return true;
	}

	return false;
}

bool reader::file::do_regex_search() {
	/***
		A regex search in Polonius should happen this way:
			0. Validate the regular expression (TODO: expression currently not validated before running)
			1. Parse the regular expression into its component parts
				e.g.:
					Expression: 'abc[a-z]+235'
					Should be split into:
						'a' 'b' 'c'
						'[a-z]+'
						'2' '3' '5'
					And recombined into multiple expressions:
						a. 'abc[a-z]+235'
						b. 'abc[a-z]+23'
						c. 'abc[a-z]+2'
						d. 'abc[a-z]+'
						e. 'abc'
						f. 'ab'
						g. 'a'
			2. Search the loaded block (of size block_size) for a match for the full expression (a)
				If found, skip to the final step
				If not found:
			3. Search: Does the loaded block END WITH a partial match? (Any of the expressions listed above after expression a)
				If no, load block #2 and go back to step #2
				If yes:
			4. Load a new block (of size block_size) STARTING FROM the start position of the aforementioned partial match, and
				restart the process HERE from step #2
			
			Final:
				Report the found match
			
		One important restriction is that we will be limited to finding regex matches no longer than the user-specified block size
			(default 10KB)
		
		TODO:
			At the moment, it's also possible to construct situations in which there is a match present, but Polonius will not be able to find it.
			Consider the expression:
				([C-Z]{2})E
			Run on a file with the contents:
				0ABCDEFGHIJKLMNOPQRSTUVWXYZ
			With a block size of 4 bytes

			The proper match would be:
				CDE
				3 5

			Polonius scans the first block for a match of the full expression:
				0ABC
			None found, it checks if it ends for a PARTIAL MATCH of the expression's first component ([C-Z]{2}), and finds that it does not
			Polonius then moves on to the next block:
				DEFG
			It finds that this block ends with a partial match ([C-Z]{2})... etc
			But, we've already missed our full match (CDE)
	***/
	int64_t match_start = 0;
	int64_t match_end = 0;

	vector<string> sub_expressions = create_sub_expressions(search_query);

	for (int64_t i = start_position; i < end_position; (i = i + block_size)) {
		regex_scan:
		int64_t amount_left_in_file = (end_position - i);
		
		if (block_size > amount_left_in_file) {
			block_size = amount_left_in_file;
		}
		
		string block_data = read(i, block_size);
		smatch regex_search_result;
		regex expression(search_query);

		if (regex_search(block_data, regex_search_result, expression)) {
				match_start = regex_search_result.prefix().length();
				match_end = (block_size - regex_search_result.suffix().length());
		} else {
			for (int64_t j = 0; j < sub_expressions.size(); j++) {
				smatch sub_expression_search_result;
				regex sub_expression(sub_expressions[j] + R"($)"); // 'R"($)"' signifies that the string must END with the match

				// Partial match found?
				bool partial_match_found = regex_search(block_data, sub_expression_search_result, sub_expression);
				int64_t partial_match_position = sub_expression_search_result.prefix().length();

				if (partial_match_found && partial_match_position > 0) {
					i = i + partial_match_position;
					goto regex_scan;
				}
			}
			continue;
		}
			// ABSOLUTE start & end position
			// (that is, relative to the start of the file)
			match_start = (i + match_start);
			match_end = (i + match_end);
			
			if (just_outputting_positions) {
				cout << match_start << " " << match_end-1 << endl;
				return true;
			}
			
			cout << regex_search_result[0] << endl;
			return true;
	}
	return false;
}

bool reader::file::do_search_job() {
	if (query_type == t_normal_search) {
		return do_normal_search();
	} else if (query_type == t_regex_search) {
		return do_regex_search();
	}
	return false;
}

bool reader::file::do_job() {
	if (!initialized) {
		cout << "Error reading file" << endl;
		return false;
	}
	
	// Make sure we're not trying to read outside the bounds of the file
	// And fail open if so
	
	// First check the start position
	if (start_position > file_length) {
		// And just read the last byte
		start_position = (file_length - 2);
	}
	
	// Now check the end position
	end_position = (start_position + amount_to_read);
	
	if (end_position > file_length) {
		// Just set it to read till the end of the file & not further
		amount_to_read = (file_length - start_position);
		end_position = (start_position + amount_to_read);
	}

	if (job == read_job) {
		return do_read_job();
	}
	
	if (job == search_job) {
		return do_search_job();
	}

	return false;
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

void reader::file::set_search_query(string query) {
	search_query = query;
}

void reader::file::set_search_type(search_type normal_or_regex) {
	query_type = normal_or_regex;
}

void reader::file::set_job_type(job_type input_job) {
	job = input_job;
}
