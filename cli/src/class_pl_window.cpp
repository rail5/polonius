#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

#ifndef IOSTREAM
	#define IOSTREAM
	#include <iostream>
#endif

using namespace std;

bool polonius::pl_window::is_initialized() {
	return initialized;
}

void polonius::pl_window::handle_force_quit(sig_atomic_t signal) {
	close();
	exit(EXIT_INTERRUPT);
}

void polonius::pl_window::handle_suspend(sig_atomic_t signal) {
	close();
	cout << "Use 'fg' to return to polonius" << endl;
	kill(0, SIGSTOP);
}

void polonius::pl_window::handle_resume(sig_atomic_t signal) {
	setup_terminal();
	
	/* Insert a fake keystroke, to neutralize a key-eating issue. */
	/* Many thanks to the nano source code for this fix */
	refresh();
	ungetch(KEY_FRESH);
}

void polonius::pl_window::setup_terminal() {
	initscr();
	cbreak();
	noecho();
	keypad(stdscr, TRUE);
}

void polonius::pl_window::resize() {
	/* Update after user resized terminal */
	/* Get the maximum number of characters that can fit in our window */
	getmaxyx(stdscr, height, width);
	maximum_number_of_chars_on_screen = (height * width);
	wresize(polonius_window, height, width);
	attached_cursor.set_limits(height, width);
	werase(polonius_window);
	refresh();
	write_from_file();
	attached_cursor.move(0, 0);
}

void polonius::pl_window::init(string file_path) {
	setup_terminal();
	
	/* Get the maximum number of characters that can fit in our window */
	getmaxyx(stdscr, height, width);
	maximum_number_of_chars_on_screen = (height * width);
	
	polonius_window = newwin(height, width, 0, 0);
	
	attached_cursor = create_new_cursor();
	attached_cursor.set_limits(height, width);
	
	attached_file = open_file(file_path);
	
	refresh();
	write_from_file();
	attached_cursor.move(0, 0);
	
	initialized = true;
}

void polonius::pl_window::write_from_file() {
	
	string file_contents = attached_file.read(0, maximum_number_of_chars_on_screen);
	
	put_string(file_contents, false);
}

void polonius::pl_window::raw_move_cursor(int y, int x) {
	wmove(polonius_window, y, x);
	
	refresh_window();
}

void polonius::pl_window::refresh_window() {
	wrefresh(polonius_window);
}

void polonius::pl_window::close() {
	endwin();
}

void polonius::pl_window::move_cursor(movement_plane dimension, int how_far) {
	int y = attached_cursor.get_y_coordinate();
	int x = attached_cursor.get_x_coordinate();
	
	if (dimension == horizontal) {
		x = (x + how_far);
	} else if (dimension == vertical) {
		y = (y + how_far);
	}
	
	attached_cursor.move(y, x);
}

void polonius::pl_window::put_char(int ch, bool and_move_cursor) {
	waddch(polonius_window, ch);
	
	if (and_move_cursor) {
		move_cursor(horizontal, 1);
	}
}

void polonius::pl_window::put_string(string &input, bool and_move_cursor) {

	attached_text_display.reset();
	attached_text_display.data = input;
	
	int amount_to_read = min((int)input.length(), maximum_number_of_chars_on_screen);
	
	for (int i = 0; i < amount_to_read; i++) {
		put_char(input.at(i));
		
		if (input.at(i) == '\n') {
			attached_text_display.rows++;
		} else {
			attached_text_display.cols_in_row[attached_text_display.rows]++;
		}
		
		if (attached_text_display.cols_in_row[attached_text_display.rows] == width) {
			attached_text_display.rows++;
		}
		
		if (attached_text_display.rows == height) {
			break; // Stop printing
		}
	}
	
	if (!and_move_cursor) {
		move_cursor(horizontal, -input.length());
	}
}

void polonius::pl_window::handle_updates() {
	int ch;
	while (true) {
		/*
		Update current cursor position
		*/
		int y = attached_cursor.get_y_coordinate();
		int x = attached_cursor.get_x_coordinate();
		raw_move_cursor(y, x);

		ch = getch();
		// User pressed a key
		switch(ch) {
			case KEY_FRESH:
				/* Ignore this keystroke */
				break;
			case KEY_RESIZE:
				/* When the user resizes their terminal,
				getch() will receive this key */
				resize();
				break;
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
			case KEY_BACKSPACE:
				attached_cursor.move(y, x-1);
				break;
			default:
				put_char(ch);
				break;
		}
	}
}


int polonius::pl_window::get_height() {
	return height;
}

int polonius::pl_window::get_width() {
	return width;
}

polonius::pl_window create_window(string open_file_path) {
	polonius::pl_window new_window;
	
	new_window.init(open_file_path);
	
	return new_window;
}
