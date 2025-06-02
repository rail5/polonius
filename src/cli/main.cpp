/**
 * Copyright (C) 2025 rail5
 */

#include <ftxui/dom/elements.hpp>
#include <ftxui/screen/screen.hpp>
#include <ftxui/component/component.hpp>
#include <ftxui/component/screen_interactive.hpp>
#include <iostream>
#include <string>

#include <getopt.h>

#include "../polonius.h"
#include "../read/reader.h"
#include "../edit/editor.h"

#include "../shared/version.h"
#include "../shared/parse_block_units.h"
#include "../shared/is_number.h"
 
int main(int arc, char* argv[]) {
	Polonius::File file;
	Polonius::reader_mode = true;
	Polonius::editor_mode = true;

	// Create an FTXUI interactive screen
	ftxui::ScreenInteractive screen = ftxui::ScreenInteractive::TerminalOutput();
	// Set up the main component
	ftxui::Component main_component = ftxui::Container::Vertical({
		ftxui::Button("Open File", [&]() {
			// Logic to open a file
			std::cout << "Open File button clicked." << std::endl;
		}),
		ftxui::Button("Read File", [&]() {
			// Logic to read a file
			std::cout << "Read File button clicked." << std::endl;
		}),
		ftxui::Button("Edit File", [&]() {
			// Logic to edit a file
			std::cout << "Edit File button clicked." << std::endl;
		}),
		ftxui::Button("Exit", [&]() {
			screen.Exit();
		})
	});
	screen.Loop(main_component);
}
