#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

inline bool file_exists(std::string name) {
	return (access(name.c_str(), F_OK) != -1);
}
