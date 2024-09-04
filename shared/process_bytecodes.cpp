/***
 * Copyright (C) 2024 rail5
*/

#ifndef REGEX
	#define REGEX
	#include <boost/regex.hpp>
#endif
#ifndef SSTREAM
	#define SSTREAM
	#include <sstream>
#endif

std::string process_bytecodes(std::string input) {
	/***
	string process_bytecodes(std::string input):
		Processes strings containing escaped byte-codes (such as \x00 or \xFF)
		
		Returns the string with all byte-codes converted to the actual bytes they represent
	***/
	boost::regex expression("(\\\\x)([a-fA-F0-9]{2})"); // Matches \x00 through \xFF

	while (boost::regex_search(input, expression)) {
		std::istringstream isolated_hex_code(boost::regex_replace(input, expression, "$2", boost::regex_constants::format_no_copy | boost::regex_constants::format_first_only)); // Isolate the first match and copy it into a stringstream

		unsigned char real_byte;

		unsigned int c;

		if (isolated_hex_code >> std::hex >> c) {
			real_byte = c; // Convert the hex code to the actual byte it represents
		}

		std::string string_real_byte(1, static_cast<char>(real_byte)); // Move said byte into a 1-character string

		input = boost::regex_replace(input, expression, string_real_byte, boost::regex_constants::format_first_only);
	}

	return input;
}
