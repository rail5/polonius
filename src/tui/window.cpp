/**
 * Copyright (C) 2025 rail5
 */

#include "window.h"

Polonius::Window::Window() : screen(nullptr) {
	initscr(); // Initialize ncurses
	cbreak(); // Disable line buffering
	noecho(); // Don't echo input characters
	keypad(stdscr, TRUE); // Enable special keys
	start_color(); // Start color functionality
	use_default_colors(); // Use default terminal colors
	screen = stdscr;

	initialize();
}

Polonius::Window::~Window() {
	endwin(); // End ncurses mode
}

void Polonius::Window::initialize() {
	// Add the bottom pane of the window displaying some basic information
	if (!screen) {
		throw std::runtime_error("Screen is not initialized");
	}

	std::shared_ptr<Polonius::TUI::HelpPane> bottom_pane = std::make_shared<Polonius::TUI::HelpPane>
		(Polonius::TUI::BOTTOM, Polonius::TUI::FULL, 4, "Polonius");
	bottom_pane->setBottomLabel("New File");
	widgets.push_back(bottom_pane);
	
}

void Polonius::Window::drawWidgets() {
	if (!screen) {
		return; // Ensure screen is initialized
	}
	for (auto& widget : widgets) {
		widget->draw(screen);
	}
}

int Polonius::Window::run() {
	int ch;
	int x = 0, y = 0;

	drawWidgets();

	// Move cursor to 0,0
	move(0, 0);

	while ((ch = getch())) {
		erase();
		attroff(A_REVERSE); // Turn off reverse attribute
		drawWidgets(); // Redraw widgets on each key press
		refresh();

		switch (ch) {
			case KEY_RESIZE:
				// Handle window resize
				endwin();
				refresh();
				clear();
				drawWidgets();
				move(y, x); // Move cursor to the current position after resize
				break;
			case KEY_UP:
				if (y > 0) y--;
				break;
			case KEY_DOWN:
				if (y < LINES - 1) y++;
				break;
			case KEY_LEFT:
				if (x > 0) x--;
				break;
			case KEY_RIGHT:
				if (x < COLS - 1) x++;
				break;
			case 24: // Ctrl+X
				clear();
				return 0; // Exit the application
			default:
				// Handle other keys or input
				buffer += static_cast<char>(ch);
				break;
		}
		move(y, x);
		// Highlight the current position
		attron(A_REVERSE);
	}

	clear();
	return 0;
}

void Polonius::Window::setFile(Polonius::File* file) {
	this->file = file;

	// Populate the initial buffer
	if (file) {
		buffer = file->readFromFile(0, static_cast<int64_t>(Polonius::block_size));
	}
}

void Polonius::Window::addWidget(std::shared_ptr<Polonius::TUI::Widget> widget) {
	widgets.push_back(widget);
}
