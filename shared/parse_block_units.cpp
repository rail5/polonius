/***
 * Copyright (C) 2024 rail5
*/

#include "is_number.h"
#include "to_lower.h"
#include <stdexcept>

/**
 * @brief Parses a string representing block units into a uint64_t value.
 * 
 * String should be in a format such as:
 * 		10K
 * 		5M
 * 		2G
 * 		3
 * 
 * The function will return the number of bytes represented by the string.
 * 
 * @param user_input The string to parse.
 * @return uint64_t The number of bytes represented by the string.
 * @throws std::invalid_argument If the input string is not in a valid format.
 * @throws std::out_of_range If the parsed value exceeds the maximum value for uint64_t.
 */
uint64_t parse_block_units(std::string user_input) {
	user_input = to_lower(user_input);

	if (user_input.empty()) {
		throw std::invalid_argument("Input string is empty");
	}

	uint64_t multiplier = 1;

	switch (user_input.back()) {
		case 'k':
			multiplier = 1024;
			user_input.pop_back();
			break;
		case 'm':
			multiplier = 1024 * 1024;
			user_input.pop_back();
			break;
		case 'g':
			multiplier = 1024 * 1024 * 1024;
			user_input.pop_back();
			break;
		case '0' ... '9':
			// No multiplier, just a number
			break;
		default:
			throw std::invalid_argument("Invalid unit in input string: " + user_input);
	}

	if (!is_number(user_input)) {
		throw std::invalid_argument("Input string is not a valid number: " + user_input);
	}

	uint64_t value = std::stoull(user_input);
	if (value > UINT64_MAX / multiplier) {
		throw std::out_of_range("Parsed value exceeds maximum uint64_t value");
	}
	return value * multiplier;
}
