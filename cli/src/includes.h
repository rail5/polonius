#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

#ifndef SSTREAM
	#define SSTREAM
	#include <sstream>
#endif

#ifndef ARRAY
	#define ARRAY
	#include <array>
#endif

#ifndef VECTOR
	#define VECTOR
	#include <vector>
#endif

#ifndef MAP
	#define MAP
	#include <map>
#endif

#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

#ifndef SIGNAL_H
	#define SIGNAL_H
	#include <signal.h>
#endif

#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
#endif

#ifdef __OpenBSD__
	// On OpenBSD, 'WEXITSTATUS' (used in polonius::command) is declared in sys/wait.h
	// TODO: Check FreeBSD
	#include <sys/wait.h>
#endif

#include "../../shared/definitions.h"

#include "namespace_polonius.h"
#include "class_cursor.cpp"
#include "class_pl_file.cpp"
#include "class_pl_text_display.cpp"
#include "class_pl_window.cpp"
