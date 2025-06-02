/***
 * Copyright (C) 2024 rail5
*/

#ifndef SYS_STAT
	#define SYS_STAT
	#include <sys/stat.h>
#endif

inline bool is_directory(std::string path) {
	struct stat s;
	if (stat(path.c_str(), &s) == 0) {
		return (s.st_mode & S_IFDIR);
	} else {
		// Error
		return false;
	}
}
