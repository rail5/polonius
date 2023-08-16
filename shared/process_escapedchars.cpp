#ifndef VECTOR
	#define VECTOR
	#include <vector>
#endif

std::string process_escapedchars(std::string input) {
	/***
	string process_escapedchars(std::string input):
		Processes strings containing escaped character sequences (literal '\n', '\t', and '\\')
		
		Returns the string with all escaped character sequences converted to the actual characters they represent
	***/
	std::vector<std::string> list_special_chars = {"\\n", "\\t", "\\\\"};
	std::vector<std::string> replacements_for_special_chars = {"\n", "\t", "\\"};
	
	for (int i = 0; i < list_special_chars.size(); i++) {
		size_t current_position = 0;
		while ((current_position = input.find(list_special_chars[i], current_position)) != std::string::npos) {
			input.replace(current_position, list_special_chars[i].length(), replacements_for_special_chars[i]);
			current_position += replacements_for_special_chars[i].length();
		}
	}

	return input;
}