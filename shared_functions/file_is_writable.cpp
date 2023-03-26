#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif

#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

inline bool file_is_writable(std::string filename) {

	#ifdef _WIN32
	return true;
	#else
	
	return (access(filename.c_str(), W_OK) == 0);
	#endif
}
