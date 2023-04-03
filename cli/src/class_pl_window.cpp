#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

bool polonius::pl_window::is_initialized() {
	return initialized;
}

void polonius::pl_window::init() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
	
	getmaxyx(stdscr, height, width);
	
	polonius_window = newwin(height, width, 0, 0);
	
	attached_cursor = create_new_cursor();
	
	initialized = true;
}

void polonius::pl_window::move_cursor(int y, int x) {
	wmove(polonius_window, y, x);
	
	refresh_window();
}

void polonius::pl_window::refresh_window() {
	wrefresh(polonius_window);
}

void polonius::pl_window::handle_updates() {
	int ch;
	nodelay(stdscr, TRUE);
	
	for (;;) {
		/*
		Update current cursor position
		*/
		int y = attached_cursor.get_y_coordinate();
		int x = attached_cursor.get_x_coordinate();
		move_cursor(y, x);
		
		if ((ch = getch()) == ERR) {
			// No input from user
			continue;
		} else {
			// User pressed a key
			switch(ch) {
				case KEY_LEFT:
					attached_cursor.move(y, x-1);
					break;
				case KEY_RIGHT:
					attached_cursor.move(y, x+1);
					break;
				case KEY_UP:
					attached_cursor.move(y-1, x);
					break;
				case KEY_DOWN:
					attached_cursor.move(y+1, x);
					break;
			}
		}
	}
}


int polonius::pl_window::get_height() {
	return height;
}

int polonius::pl_window::get_width() {
	return width;
}

polonius::pl_window create_window() {
	polonius::pl_window new_window;
	
	new_window.init();
	
	return new_window;
}
