#ifndef SYS_STAT_H
	#define SYS_STAT_H
	#include <sys/stat.h>
#endif

inline bool file_exists(std::string name) {
	struct stat buffer;
	return (stat (name.c_str(), &buffer) == 0);
}
