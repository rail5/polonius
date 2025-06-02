/***
 * Copyright (C) 2024 rail5
*/

#include "to_lower.h"
#include <algorithm>

std::string to_lower(const std::string& input) {
	std::string output = input;
	std::transform(output.begin(), output.end(), output.begin(),
		[](unsigned char c){ return std::tolower(c); });
	
	return output;
}
