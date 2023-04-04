#include "includes.h"

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

#ifndef UNISTD_H
	#define UNISTD_H
	#include <unistd.h>
#endif

#ifndef SIGNAL_H
	#define SIGNAL_H
	#include <signal.h>
#endif

using namespace std;

int main(int argc, char* argv[]) {
	
	string program_name = "polonius";
	
	string program_version = "0.1";
	
	string program_author = "rail5";
	
	string helpstring = "No help here.\n";
	
	/*
	Setting up to read the provided file (if provided)
	*/
	string file_path = "";
	
	if (argc > 1) {
		file_path = string(argv[1]);
	}
	
	polonius::pl_window program_window = create_window(file_path);
	
	/*
	Prepare to catch CTRL-C, CTRL-Z, CTRL-\ events
	*/
	signal(SIGINT, program_window.handle_force_quit);
	signal(SIGTSTP, program_window.handle_force_quit);
	signal(SIGQUIT, program_window.handle_force_quit);
	
	program_window.handle_updates();
	
	program_window.close();

//	cout << polonius::command::exec("polonius-reader -i " + file_path);

	return 0;
}
