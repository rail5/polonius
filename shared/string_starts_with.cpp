inline bool string_starts_with(std::string const &haystack, std::string const &needle) {
	/* Return true if 'haystack' starts with 'needle' */
	return (!haystack.find(needle));
}
