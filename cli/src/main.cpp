#include "includes.h"

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

using namespace std;

int main(int argc, char* argv[]) {
	
	string program_name = "polonius";
	
	string program_version = "0.1";
	
	string program_author = "rail5";
	
	string helpstring = "No help here.\n";
	
	polonius::pl_window program_window = create_window();
	
	program_window.handle_updates();
	
	program_window.close();

	return 0;
}
