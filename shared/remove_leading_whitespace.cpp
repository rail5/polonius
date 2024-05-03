/***
 * Copyright (C) 2024 rail5
*/

std::string remove_leading_whitespace(const std::string &input) {
	const std::string whitespace_chars = " \n\r\t\f\v";
	
	size_t start = input.find_first_not_of(whitespace_chars);
	
	return (start == std::string::npos) ? "" : input.substr(start);
}
