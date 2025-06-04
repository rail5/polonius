/**
 * Copyright (C) 2025 rail5
 */

#include "widget.h"

Polonius::TUI::Message::Message() : message("") {}

Polonius::TUI::Message::Message(const std::string& msg) : message(msg) {}

void Polonius::TUI::Message::setMessage(const std::string& msg) {
	message = msg;
}

void Polonius::TUI::Message::setColor(Polonius::TUI::Color color) {
	this->color = color;
}

void Polonius::TUI::Message::draw() {
	if (message.empty()) {
		return; // Nothing to display
	}
	
	WINDOW* window = parent->getScreen();
	// Print the message at the ABSOLUTE bottom of the screen
	int y = LINES;
	// Center the message horizontally (cast size_t to int to avoid warnings)
	int msg_len = static_cast<int>(message.length());
	int x = (COLS - msg_len) / 2;;

	// Highlight the message according to the color
	wattroff(window, A_REVERSE);
	wattron(window, COLOR_PAIR(color));
	mvwprintw(window, y - 1, x, "%s", message.c_str()); // Print the message at the bottom of the screen
	wattroff(window, COLOR_PAIR(color)); // Turn off the color pair
	wrefresh(window);
	parent->restoreCursorPosition(); // Restore cursor position after drawing the message
}
