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

#ifndef GETOPT_H
	#define GETOPT_H
	#include <getopt.h>
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
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] =
	{
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "h", long_options, &option_index)) != -1) {
		switch(c) {
			case 'h':
				cout << helpstring;
				return 0;
				break;
		}
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		file_path = argv[option_index];
		/*
		This has the following disadvantage:
		If a user runs the command:
			$ polonius ./somefile ./some-other-file
		This will take the last-specified (./some-other-file) and ignore the first
		Nano handles this by opening multiple "file buffers," one for each file
		Maybe we'll do that later on?
		*/
	}
	
	polonius::pl_window program_window = create_window(file_path);
	
	/*
	Prepare to catch CTRL-C (SIGINT) and CTRL-\ (SIGQUIT) events
	*/
	signal(SIGINT, program_window.handle_force_quit);
	signal(SIGQUIT, program_window.handle_force_quit);
	
	/*
	Prepare to catch CTRL-Z (SIGSTOP, "suspend") and "resume" (SIGCONT) events
	*/
	signal(SIGTSTP, program_window.handle_suspend);
	signal(SIGCONT, program_window.handle_resume);
	
	program_window.handle_updates();
	
	program_window.close();

//	cout << polonius::command::exec("polonius-reader -i " + file_path);

	return 0;
}
