/***
 * Copyright (C) 2024 rail5
*/

#include "process_special_chars.h"
#include <string>

/**
 * @brief Substitutes escape sequences in the input string with their corresponding characters.
 * 
 * E.g, \x00 will be replaced with a null character, etc.
 * 
 * This processes \x00 through \xFF, as well as \n, \t, and \\.
 */
std::string process_special_chars(const std::string& input) {
	std::string result;
	result.reserve(input.length());
	
	for (size_t i = 0; i < input.length(); i++) {
		switch (input[i]) {
			case '\\':
				// Check the next character
				if (i + 1 < input.length()) {
					switch (input[i + 1]) {
						case 'n':
							result += '\n';
							i++; // Skip the next character
							break;
						case 't':
							result += '\t';
							i++;
							break;
						case '\\':
							result += '\\';
							i++;
							break;
						case 'x': {
							// Handle hex escape sequences like \x00 to \xFF
							if (i + 3 < input.length() && 
								std::isxdigit(input[i + 2]) && 
								std::isxdigit(input[i + 3])) {
								std::string hex_str = input.substr(i + 2, 2);
								char hex_char = static_cast<char>(std::stoi(hex_str, nullptr, 16));
								result += hex_char;
								i += 3; // Skip the next two characters and the 'x'
							} else {
								result += '\\'; // Just add the backslash if not a valid hex sequence
							}
							break;
						}
						default:
							// If it's not a recognized escape sequence, just add the backslash
							result += '\\';
							break;
					}
				} else {
					// If the backslash is at the end of the string, just add it
					result += '\\';
				}
				break;
			default:
				result += input[i];
				break;
		}
	}
	
	return result;
}
