/***
 * Copyright (C) 2024 rail5
*/

#ifndef FN_IS_NUMBER
	#define FN_IS_NUMBER
	#include "is_number.cpp"
#endif

inline int parse_block_units(const std::string &user_input) {
	/***
	inline int parse_block_units(std::string user_input):
		Parses "block_size" strings such as '10K', '16M', etc
		
		Returns the equivalent size measured in bytes
		
		Returns -1 if user_input string is invalid
	***/
	std::string last_character = user_input.substr(user_input.length()-1, user_input.length()-1);
	
	std::string all_but_last_character = user_input.substr(0, user_input.length()-1);
	
	if (!is_number(all_but_last_character) && user_input.length() > 1) {
		return -1;
	}
	
	if (last_character == "K" || last_character == "k") {
		int blocksize = stoi(all_but_last_character);
		return (blocksize * 1024);
	}
	
	if (last_character == "M" || last_character == "m") {
		int blocksize = stoi(all_but_last_character);
		return (blocksize * 1024 * 1024);
	}
	
	/*
	If we've made it this far,
	verify user_input is all digits and just pass the original value through
	*/
	
	if (!is_number(user_input)) {
		return -1;
	}
	return static_cast<int>(stol(user_input));
}
