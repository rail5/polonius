std::vector<std::string> explode(std::string const &input, char delimiter, int maximum_number_of_elements = 0) {
	/***
	vector<string> explode(string &input, char delimiter, int maximum_number_of_elements):
		Similar to PHP's "explode" function
		
		Splits the input string into separate strings, using the provided delimiter
			Example:
				string input = "Test,one,two,three";
				explode(input, ",");
			Would return the following vector:
				{"Test", "one", "two", "three"}
				
		The "maximum_number_of_elements" parameter, if set higher than zero, limits how many splits we can do
			Example:
				string input = "Test,one,two,three";
				explode(input, ",", 2);
			Would return the following vector:
				{"Test", "one", "two,three"}
	***/
	// Create the vector
	std::vector<std::string> output_vector;
	
	// Create a stringstream from the input
	std::istringstream input_string_stream(input);
	
	// Cycle through the stringstream and push back the substrings
	for (std::string token; std::getline(input_string_stream, token, delimiter); ) {
		output_vector.push_back(std::move(token));
	}
	
	// If maximum_number_of_elements is set, we want to recombine any splits after the maximum
	if (maximum_number_of_elements > 0) {
	
		// Set the highest index (counting from zero) we want the vector to have
		int last_permissible_element = maximum_number_of_elements - 1;
		
		// Store the current actual highest index of the vector
		int last_element = output_vector.size() - 1;
		
		// Cycle through the vector and combine into the last_permissible_element
		// (re-inserting the delimiter character between previously split elements)
		// Then delete the higher elements
		for (int i = maximum_number_of_elements; i <= last_element; i++) {
			output_vector[last_permissible_element] = output_vector[last_permissible_element] + delimiter + output_vector[last_permissible_element + 1];
			output_vector.erase(output_vector.begin() + (last_permissible_element + 1));
		}
	}
	
	return output_vector;
}
