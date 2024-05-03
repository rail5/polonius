/***
 * Copyright (C) 2024 rail5
*/

#ifndef ALGORITHM
	#define ALGORITHM
	#include <algorithm>
#endif

inline std::string to_lower(const std::string& input) {
	std::string output = input;
	std::transform(output.begin(), output.end(), output.begin(),
		[](unsigned char c){ return std::tolower(c); });
	
	return output;
}
