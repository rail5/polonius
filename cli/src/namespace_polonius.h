#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

namespace polonius {
	
	/*
	Forward declarations of classes:
	*/
	class cursor;
	class pl_window;
	
	enum movement_plane {
		horizontal,
		vertical
	};
	
	class cursor {
		private:
			bool initialized = false;
			int y_coordinate = 0;
			int x_coordinate = 0;
		public:
			void move(int y, int x);
			void set_initialized(bool set_value);
			
			int get_y_coordinate();
			int get_x_coordinate();
			
			bool can_move_left();
	};

	class pl_window {
		private:
			bool initialized = false;
			WINDOW* polonius_window;
			
			int height = 0;
			int width = 0;
			
			void raw_move_cursor(int y, int x);
		public:
			bool is_initialized();
		
			void init();
			void refresh_window();
			void close();
			
			cursor attached_cursor;
			
			void move_cursor(movement_plane dimension, int how_far);
			
			void put_char(int ch);
			
			void handle_updates();
			
			int get_height();
			int get_width();
	};

}
