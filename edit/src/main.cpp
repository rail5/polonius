/**
 * Copyright (C) 2023-2025 rail5
 */

#include <iostream>
#include <vector>
#include <string>

#include <getopt.h>

#include "editor.h"

int main(int argc, char* argv[]) {
	Polonius::Editor::File file;
	std::vector<std::string> instructions;

	// getopt
	int opt;
	while ((opt = getopt(argc, argv, "a:")) != -1) {
		switch (opt) {
			case 'a':
				instructions.push_back(optarg);
				break;
		}
	}
	// Handle non-option arguments
	for (int index = optind; index < argc; ++index) {
		std::string arg = argv[index];
		try {
			file = Polonius::Editor::File(arg);
		} catch (const std::exception& e) {
			std::cerr << "Error: " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}

	for (const auto& instruction : instructions) {
		try {
			file.parseInstructions(instruction);
		} catch (const std::exception& e) {
			std::cerr << "Error parsing instruction '" << instruction << "': " << e.what() << std::endl;
			return EXIT_FAILURE;
		}
	}
	for (const auto& instr : file.get_instructions()) {
		std::cout << "Instruction: " << (instr.type() == Polonius::Editor::INSERT ? "INSERT" : instr.type() == Polonius::Editor::REMOVE ? "REMOVE" : "REPLACE")
		          << ", Start: " << instr.start() << ", End: " << instr.end()
		          << ", Text: '" << instr.get_text() << "'" << std::endl;
	}
	return EXIT_SUCCESS;
}
