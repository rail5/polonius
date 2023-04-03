#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

using namespace std;

namespace polonius {
	class window {
		private:
			bool initialized = false;
			WINDOW* polonius_window;
			
			int height = 0;
			int width = 0;
		public:
			void init();
			void refresh_window();
			void move_cursor(int y_coordinate, int x_coordinate);
			
			int get_height();
			int get_width();
	};
	
	window create_window();

}
