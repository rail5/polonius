void string_replace_all(std::string& haystack, const std::string& needle, const std::string& replace_with) {
	std::string output_string;
	output_string.reserve(haystack.length());  // avoids a few memory allocations

	std::string::size_type lastPos = 0;
	std::string::size_type findPos;

	while(std::string::npos != (findPos = haystack.find(needle, lastPos))) {
		output_string.append(haystack, lastPos, findPos - lastPos);
		output_string += replace_with;
		lastPos = findPos + needle.length();
	}

	// Care for the rest after last occurrence
	output_string += haystack.substr(lastPos);

	haystack.swap(output_string);
}
