/***
 * Copyright (C) 2023-2025 rail5
*/

#include <iostream>
#include <string>

#include <getopt.h>

#include "../polonius.h"
#include "../file.h"
#include "reader.h"

#include "../shared/version.h"
#include "../shared/parse_block_units.h"
#include "../shared/is_number.h"
#include "../shared/process_special_chars.h"

int main(int argc, char* argv[]) {
	Polonius::reader_mode = true;
	Polonius::editor_mode = false; // Disable editor mode for the reader
	Polonius::exit_code = EXIT_SUCCESS;
	Polonius::File file;

	const char* help_string = "polonius-reader " program_version "\n"
		"Usage: polonius-reader <file> [options]\n"
		"Options:\n"
		"  -s, --start <position>       Start reading from this position (default: 0)\n"
		"  -l, --length <length>        Read this many bytes (default: until EOF)\n"
		"  -b, --block-size <size>      Set the block size for reading (default: 10K)\n"
		"  -f, --find <string>		    Search for this string in the file\n"
		"                                If -s is set, start searching from that position\n"
		"                                If -l is set, search only within that length\n"
		"  -e, --regex                  Treat the search string as a regular expression\n"
		"  -c, --special-chars          Process escaped characters in the search string\n"
		"                                (\\n, \\t, \\\\, \\x00 through \\xFF)\n"
		"  -p, --position               Output start and end position rather than text\n"
		"                                If -f is set, output position of the match\n"
		"  -h, --help                   Show this help message and exit\n"
		"  -V, --version                Show version information and exit\n";

	const char* version_string = "polonius-reader " program_version "\n"
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
		{"start", required_argument, nullptr, 's'},
		{"length", required_argument, nullptr, 'l'},
		{"block-size", required_argument, nullptr, 'b'},
		{"find", required_argument, nullptr, 'f'},
		{"regex", no_argument, nullptr, 'e'},
		{"special-chars", no_argument, nullptr, 'c'},
		{"position", no_argument, nullptr, 'p'},
		{"help", no_argument, nullptr, 'h'},
		{"version", no_argument, nullptr, 'V'},
		{nullptr, 0, nullptr, 0} // Sentinel
	};

	std::string search_query;

	while ((opt = getopt_long(argc, argv, "b:cef:hl:ps:V", long_options, nullptr)) != -1) {
		switch (opt) {
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
				Polonius::special_chars = true;
				break;
			case 'e':
				Polonius::Reader::search_mode = Polonius::Reader::t_regex_search;
				break;
			case 'f':
				search_query = optarg;
				break;
			case 'h':
				std::cout << help_string;
				return EXIT_SUCCESS;
			case 'l':
				if (!is_number(optarg)) {
					std::cerr << "Error: Length must be a number." << std::endl;
					return EXIT_FAILURE;
				}
				Polonius::Reader::amount_to_read = std::stoll(optarg);
				if (Polonius::Reader::amount_to_read <= 0) {
					std::cerr << "Error: Length must be greater than zero" << std::endl;
					return EXIT_FAILURE;
				}
				break;
			case 'p':
				Polonius::Reader::output_positions = true;
				break;
			case 's':
				if (!is_number(optarg)) {
					std::cerr << "Error: Start position must be a number." << std::endl;
					return EXIT_FAILURE;
				}
				Polonius::Reader::start_position = std::stoull(optarg);
				break;
			case 'V':
				std::cout << version_string;
				return EXIT_SUCCESS;
			case '?':
				if (optopt == 'b' || optopt == 'f' || optopt == 'l' || optopt == 's') {
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

	if (!received_file) {
		std::cerr << "Error: No file specified." << std::endl;
		std::cerr << help_string;
		return EXIT_FAILURE;
	}

	try {
		// Calculate amount to read
		if (Polonius::Reader::amount_to_read < 0) {
			uint64_t eof = file.getSize();
			Polonius::Reader::amount_to_read = static_cast<int64_t>(eof - Polonius::Reader::start_position);
		}

		// Search
		if (!search_query.empty()) {
			// Searching
			Polonius::Block search_result;

			if (Polonius::special_chars) {
				search_query = process_special_chars(search_query);
			}

			switch (Polonius::Reader::search_mode) {
				case Polonius::Reader::t_normal_search:
					search_result = file.search(Polonius::Reader::start_position, Polonius::Reader::amount_to_read, search_query);
					break;
				case Polonius::Reader::t_regex_search:
					search_result = file.regex_search(Polonius::Reader::start_position, Polonius::Reader::amount_to_read, search_query);
					break;
			}

			if (search_result.initialized()) {
				if (Polonius::Reader::output_positions) {
					std::cout << search_result.start() << " " << search_result.end() << std::endl;
				} else {
					std::cout << search_result.get_contents() << std::endl;
				}
				return EXIT_SUCCESS;
			} else {
				return EXIT_FAILURE;
			}
		}

		// Normal read
		Polonius::Block read_result = file.readFromFile(Polonius::Reader::start_position,
			std::min(static_cast<int64_t>(Polonius::block_size), Polonius::Reader::amount_to_read));
		
		uint64_t requested_end_position = Polonius::Reader::start_position + static_cast<uint64_t>(Polonius::Reader::amount_to_read);

		uint64_t actual_end_position = read_result.end();

		uint64_t actual_start_position = Polonius::Reader::start_position;
		if (actual_start_position >= file.getSize()) {
			actual_start_position = 0;
		}

		while (read_result.initialized() && read_result.end() < requested_end_position) {
			actual_end_position = read_result.end();
			if (!Polonius::Reader::output_positions) {
				std::cout << read_result.get_contents();
			}
			read_result = file.readFromFile(read_result.end() + 1,
				std::min(static_cast<int64_t>(Polonius::block_size), Polonius::Reader::amount_to_read));
		}
		if (Polonius::Reader::output_positions) {
			std::cout << actual_start_position << " " << actual_end_position << std::endl;
		}

	} catch (const std::exception& e) {
		std::cerr << "Error reading file: " << e.what() << std::endl;
		return EXIT_FAILURE;
	}

	return Polonius::exit_code;
}
