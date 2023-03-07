inline std::string isolate_path_from_filename(std::string filename) {
	std::string path = filename.substr(0, filename.find_last_of("/\\") + 1);
	
	if (path == "") {
		path = std::filesystem::current_path();
	}
	
	return path;
}
