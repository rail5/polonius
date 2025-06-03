/**
 * Copyright (C) 2025 rail5
 */

#include "window.h"

Polonius::Window::Window() : NCursesApplication(true), screen(nullptr) {
	initscr(); // Initialize ncurses
	cbreak(); // Disable line buffering
	noecho(); // Don't echo input characters
	keypad(stdscr, TRUE); // Enable special keys
	start_color(); // Start color functionality
	screen = new NCursesWindow(stdscr); // Initialize screen after initscr()
}

Polonius::Window::~Window() {
	endwin(); // End ncurses mode
	delete screen; // Clean up the screen
}

int Polonius::Window::run() {
	int ch;
	int x = 0, y = 0;

	while ((ch = getch()) != 'q') { // Exit on 'q'
		switch (ch) {
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
			default:
				buffer += static_cast<char>(ch);
				break;
		}
		screen->move(y, x);
		// Highlight the current position
		screen->attron(A_REVERSE);
	}
	screen->clear();
	return 0;
}
