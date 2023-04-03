#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

void polonius::cursor::move(int y, int x) {
	y_coordinate = y;
	x_coordinate = x;
}

void polonius::cursor::set_initialized(bool set_value) {
	initialized = set_value;
}

int polonius::cursor::get_y_coordinate() {
	return y_coordinate;
}

int polonius::cursor::get_x_coordinate() {
	return x_coordinate;
}

polonius::cursor create_new_cursor() {
	polonius::cursor new_cursor;
	
	new_cursor.set_initialized(true);
	
	return new_cursor;
}
