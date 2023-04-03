#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

void polonius::window::init() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	getmaxyx(stdscr, height, width);
	
	polonius_window = newwin(height, width, 0, 0);
	
	initialized = true;
}

void polonius::window::refresh_window() {
	wrefresh(polonius_window);
}

void polonius::window::move_cursor(int y_coordinate, int x_coordinate) {
	wmove(polonius_window, y_coordinate, x_coordinate);
	refresh_window();
}


int polonius::window::get_height() {
	return height;
}

int polonius::window::get_width() {
	return width;
}


polonius::window create_window() {
	polonius::window new_window;
	
	new_window.init();
	
	return new_window;
}
