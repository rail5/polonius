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
	
	string helpstring = "No help here.\n";
	
	/*
	Setting up to read the provided file (if provided)
	*/
	string file_path = "";
	
	bool received_filename = false;
	
	/*
	GETOPT
	*/
	int c;
	opterr = 0;
	int option_index = 0;
	
	static struct option long_options[] =
	{
		{"input", required_argument, 0, 'i'},
		{"help", no_argument, 0, 'h'}
	};
	
	while ((c = getopt_long(argc, argv, "i:h", long_options, &option_index)) != -1) {
		switch(c) {
			case 'i':
				if (received_filename) {
					cerr << program_name + ": Error: Multiple files specified" << endl;
					return EXIT_BADFILE;
				}
				file_path = optarg;
				received_filename = true;
				break;
			case 'h':
				cout << helpstring;
				return EXIT_SUCCESS;
				break;
			case '?':
				if (optopt == 'i') {
					cerr << program_name << ": Option -" << (char)optopt << " requires an argument" << endl << "Use -h for help" << endl;
					return EXIT_BADOPT;
				}
		}
	}
	
	for (option_index = optind; option_index < argc; option_index++) {
		if (received_filename) {
			cerr << program_name + ": Error: Multiple files specified" << endl;
			return EXIT_BADFILE;
		}
		file_path = argv[option_index];
		received_filename = true;
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

	return EXIT_SUCCESS;
}
