/**
 * Copyright (C) 2023-2025 rail5
 */

#include <iostream>
#include <vector>
#include <string>
#include <fstream>

#include <getopt.h>

#include "../polonius.h"
#include "editor.h"
#include "../shared/version.h"
#include "../shared/parse_block_units.h"

int main(int argc, char* argv[]) {
	Polonius::File file;
	std::string instructions;

	const char* help_string = "polonius-editor " program_version "\n"
		"Usage: polonius-editor <file> [options]\n"
		"Options:\n"
		"  -a, --add <instruction-sequence> Add an instruction sequence to the file\n"
		"                                    One instruction per line\n"
		"                                    Format:\n"
		"                                    [INSERT|REMOVE|REPLACE] <start> <text|end>\n"
		"  -f, --instruction-file <file>    Read instructions from a file\n"
		"  -c, --special-chars              Process escaped chars in instructions\n"
		"                                    (\\n, \\t, \\\\, \\x00 through \\xFF)\n"
		"  -n, --no-newline                 Do not add a newline to the end of the file\n"
		"  -b, --block-size <size>          Set the block size for file operations\n"
		"                                    Default is 10K\n"
		"  -V, --version                    Show version information and exit\n"
		"  -h, --help                       Show this help message and exit\n";
	
	const char* version_string = "polonius-editor " program_version "\n"
		"Copyright (C) 2023-2025 rail5\n"
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
		{"add", required_argument, nullptr, 'a'},
		{"block-size", required_argument, nullptr, 'b'},
		{"special-chars", no_argument, nullptr, 'c'},
		{"instruction-file", required_argument, nullptr, 'f'},
		{"help", no_argument, nullptr, 'h'},
		{"no-newline", no_argument, nullptr, 'n'},
		{"version", no_argument, nullptr, 'V'},
		{nullptr, 0, nullptr, 0} // Sentinel
	};

	while ((opt = getopt_long(argc, argv, "a:b:cf:hnV", long_options, nullptr)) != -1) {
		switch (opt) {
			case 'a':
				instructions += optarg;
				instructions += '\n';
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
			case 'c':
				Polonius::Editor::special_chars = true;
				break;
			case 'f':
				{
					std::ifstream instruction_file(optarg);
					if (!instruction_file) {
						std::cerr << "Error opening instruction file: " << optarg << std::endl;
						return EXIT_FAILURE;
					}
					// Add the entire file content to the instructions string
					std::string line;
					while (std::getline(instruction_file, line)) {
						if (!line.empty()) {
							instructions += line + '\n';
						}
					}
					instruction_file.close();
				}
				break;
			case 'h':
				std::cout << help_string;
				return EXIT_SUCCESS;
			case 'n':
				Polonius::Editor::append_newline = false;
				break;
			case 'V':
				std::cout << version_string;
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'a' || optopt == 'b' || optopt == 'f') {
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
			std::cerr << "Error: Multiple files specified. Only one file can be edited at a time." << std::endl;
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

	if (!received_file) {
		std::cerr << "Error: No file specified." << std::endl;
		std::cerr << help_string;
		return EXIT_FAILURE;
	}

	try {
		file.parseInstructions(instructions);
	} catch (const std::exception& e) {
		std::cerr << "Error parsing instructions: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	try {
		file.executeInstructions();
	} catch (const std::exception& e) {
		std::cerr << "Error executing instructions: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return EXIT_SUCCESS;
}
