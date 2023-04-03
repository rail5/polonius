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
	
	polonius::window program_window = create_window();
	
	while (true) {
		// Move the cursor one step every quarter second
		
		for (int place = 0; place < program_window.get_width(); place++) {
			program_window.move_cursor(0, place);
			usleep(250000);
		}
	}

	return 0;
}
