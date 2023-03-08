#ifndef ALGORITHM
	#define ALGORITHM
	#include <algorithm>
#endif

inline std::string to_lower(std::string input) {
	std::string output = input;
	std::transform(output.begin(), output.end(), output.begin(),
		[](unsigned char c){ return std::tolower(c); });
	
	return output;
}
