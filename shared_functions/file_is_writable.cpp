#ifndef FILESYSTEM
	#define FILESYSTEM
	#include <filesystem>
#endif

#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

inline bool file_is_writable(std::string filename) {
	return (access(filename.c_str(), W_OK) == 0);
}
