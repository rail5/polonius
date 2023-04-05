#define KEY_FRESH 0X4FE

// Exit codes:

#define EXIT_SUCCESS 0
#define EXIT_BADFILE 1
#define EXIT_BADARG 2
#define EXIT_BADOPT 3
#define EXIT_OTHER 4
#define EXIT_INTERRUPT 5

#ifdef __clang__
	#define fopen64 fopen
	// clang doesn't have fopen64
	// This might(?) mean that we can't open >2GB files on 32-bit systems which use clang/llvm
	// Testing is needed
#endif
