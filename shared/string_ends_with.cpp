/***
 * Copyright (C) 2024 rail5
*/

inline bool string_ends_with(std::string const &haystack, std::string const &needle) {
	/* Return true if 'haystack' ends with 'needle' */
	if (haystack.length() >= needle.length()) {
		return (0 == haystack.compare (haystack.length() - needle.length(), needle.length(), needle));
	} else {
		return false;
	}
}
