/***
 * Copyright (C) 2024 rail5
*/

#ifndef SHARED_DEFINITIONS_H_
#define SHARED_DEFINITIONS_H_

// Basic info:
#include "version.h"
#define program_author "rail5"

// KEY_FRESH is used by the CLI for window updates
// Just a useless key that we ungetch() to overcome an input-eating issue with ncurses
#define KEY_FRESH 0X4FE

// Exit codes:
#define EXIT_SUCCESS 0
#define EXIT_BADFILE 1
#define EXIT_BADARG 2
#define EXIT_BADOPT 3
#define EXIT_OTHER 4
#define EXIT_INTERRUPT 5

// Account for systems which don't have fopen64
#if defined __clang__ || defined __HAIKU__
	#define fopen64 fopen
	// This might(?) mean that we can't open >2GB files on 32-bit systems which use clang/llvm
	// Testing is needed
#endif

#endif // SHARED_DEFINITIONS_H_
