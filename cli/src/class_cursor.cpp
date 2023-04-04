#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

void polonius::cursor::set_initialized(bool set_value) {
	initialized = set_value;
}

void polonius::cursor::move(int y, int x) {

	if (x >= x_limit) {
		x = 0;
		y = y + 1;
	}
	
	if (y >= y_limit) {
		y = 0;
	}
	
	if (x < 0) {
		x = x_limit - 1;
		y = y - 1;
	}
	
	if (y < 0) {
		y = y_limit - 1;
	//	x = 0;
	}

	y_coordinate = y;
	x_coordinate = x;
}

void polonius::cursor::set_limits(int y, int x) {
	y_limit = y;
	x_limit = x;
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
