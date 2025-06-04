/**
 * Copyright (C) 2025 rail5
 */

#include <iostream>
#include <string>

#include <getopt.h>

#include "../polonius.h"
#include "../read/reader.h"
#include "../edit/editor.h"
#include "window.h"

#include "../shared/version.h"
#include "../shared/parse_block_units.h"
#include "../shared/is_number.h"
 
int main(int argc, char* argv[]) {
	Polonius::reader_mode = true;
	Polonius::editor_mode = true;
	Polonius::File file;

	Polonius::Window window;

	const char* help_string = "polonius " program_version "\n"
		"Usage: polonius [file] [options]\n"
		"Options:\n"
		"  -s, --start <position>       Open the file at this position\n"
		"  -b, --block-size <size>      Set the block size for reading (default: 10K)\n"
		"  -h, --help                   Show this help message and exit\n"
		"  -V, --version                Show version information and exit\n";
	
	const char* version_string = "polonius " program_version "\n"
		"Copyright (C) 2025 rail5\n"
		"\n"
		"This program is free software; you can redistribute it and/or modify\n"
		"it under the terms of the GNU General Public License as published by\n"
		"the Free Software Foundation; either version 3 of the License, or\n"
		"(at your option) any later version.\n"
		"\n"
		"This program is distributed in the hope that it will be useful,\n"
		"but WITHOUT ANY WARRANTY; without even the implied warranty of\n"
		"MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the\n"
		"GNU General Public License for more details.\n"
		"\n"
		"You should have received a copy of the GNU General Public License\n"
		"along with this program. If not, see http://www.gnu.org/licenses/.\n";

	// getopt
	int opt;
	// Long options
	static struct option long_options[] = {
		{"start", required_argument, nullptr, 's'},
		{"block-size", required_argument, nullptr, 'b'},
		{"help", no_argument, nullptr, 'h'},
		{"version", no_argument, nullptr, 'V'},
		{nullptr, 0, nullptr, 0} // Sentinel
	};

	while ((opt = getopt_long(argc, argv, "s:b:hV", long_options, nullptr)) != -1) {
		switch (opt) {
			case 's':
				if (!is_number(optarg)) {
					std::cerr << "Error: Start position must be a number." << std::endl;
					return EXIT_FAILURE;
				}
				Polonius::Reader::start_position = std::stoull(optarg);
				break;
			case 'b':
				try {
					Polonius::block_size = parse_block_units(optarg);
				} catch (const std::exception& e) {
					std::cerr << "Error parsing block size: " << e.what() << std::endl;
					return EXIT_FAILURE;
				}
				if (Polonius::block_size == 0) {
					std::cerr << "Error: Block size cannot be zero." << std::endl;
					return EXIT_FAILURE;
				}
				break;
			case 'h':
				std::cout << help_string;
				return EXIT_SUCCESS;
			case 'V':
				std::cout << version_string;
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 's' || optopt == 'b') {
					std::cerr << "Option -" << static_cast<char>(optopt) << " requires an argument." << std::endl;
				} else {
					std::cerr << "Unknown option: -" << static_cast<char>(optopt) << std::endl;
				}
				std::cerr << help_string;
				return EXIT_FAILURE;
		}
	}

	// Handle non-option arguments
	bool received_file = false;
	for (int index = optind; index < argc; ++index) {
		std::string arg = argv[index];
		if (received_file) {
			std::cerr << "Error: Only one file can be specified." << std::endl;
			return EXIT_FAILURE;
		}
		try {
			file = Polonius::File(arg);
			received_file = true;
		} catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	// If we haven't received a file, we're creating a new one
	window.setFile(&file);

	return window.run();
}
