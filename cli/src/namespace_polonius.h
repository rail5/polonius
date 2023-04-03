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
	};

	class pl_window {
		private:
			bool initialized = false;
			WINDOW* polonius_window;
			
			int height = 0;
			int width = 0;
			
			void move_cursor(int y, int x);
		public:
			bool is_initialized();
		
			void init();
			void refresh_window();
			
			cursor attached_cursor;
			
			void handle_updates();
			
			int get_height();
			int get_width();
	};

}
