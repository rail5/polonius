#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

#ifndef ARRAY
	#define ARRAY
	#include <array>
#endif

#ifndef VECTOR
	#define VECTOR
	#include <vector>
#endif

#ifndef MAP
	#define MAP
	#include <map>
#endif

using namespace std;

namespace polonius {
	
	/*
	Forward declarations of classes:
	*/
	class cursor;
	class command;
	class pl_file;
	class pl_text_display;
	class pl_window;
	
	enum movement_plane {
		horizontal,
		vertical
	};
	
	struct screen_position {
		int y_coordinate;
		int x_coordinate;
		
		friend ostream &operator<<(ostream &os, const screen_position &pos) {
			os << "{" << pos.y_coordinate << "," << pos.x_coordinate << "}";
			return os;
		}
		
		bool operator==(const screen_position &other_position) const {
			return (y_coordinate == other_position.y_coordinate && x_coordinate == other_position.x_coordinate);
		}
		
		bool operator!=(const screen_position &other_position) const {
			return !(other_position == *this);
		}
		
		screen_position(int y, int x) : y_coordinate(y), x_coordinate(x) {}
	};
	
	class cursor {
		private:
			bool initialized = false;
			int y_coordinate = 0;
			int x_coordinate = 0;
			int y_limit = 0;
			int x_limit = 0;
		public:
			void set_initialized(bool set_value);
			
			void move(int y, int x);
			
			void move_left(int how_far);
			void move_right(int how_far);
			void move_up(int how_far);
			void move_down(int how_far);
			
			void set_limits(int y, int x);
			
			int get_y_coordinate();
			int get_x_coordinate();
			
			screen_position get_position();
			
			bool can_move_left();
	};
	
	struct command_result {
		string output;
		int exit_status;
		
		friend ostream &operator<<(ostream &os, const command_result &result) {
			os << result.output;
			return os;
		}
		
		bool operator==(const command_result &other_result) const {
			return (output == other_result.output && exit_status == other_result.exit_status);
		}
		
		bool operator!=(const command_result &other_result) const {
			return !(other_result == *this);
		}
	};
	
	class command {
		public:
			static command_result exec(const string &command) {
				int exit_code = 255;
				array<char, 1024> buffer {};
				string result;
				
				FILE* pipe = popen(command.c_str(), "r");
				if (pipe == nullptr) {
					throw runtime_error("Failed to open file");
				}
				try {
					size_t bytes_read;
					while ((bytes_read = fread(buffer.data(), sizeof(buffer.at(0)), sizeof(buffer), pipe)) != 0) {
						result += string(buffer.data(), bytes_read);
					}
				} catch (...) {
					pclose(pipe);
					throw;
				}
				
				#ifndef __APPLE__
				/* Getting the return code on *sensible* (POSIX) systems */
				exit_code = WEXITSTATUS(pclose(pipe));
				#else
				/* Getting the return code from those nutcases at Apple */
				exit_code = pclose(pipe);
				#endif
				return command_result{result, exit_code};
			}
	};
	
	class pl_file {
		private:
			bool initialized = false;
			string file_path = "";
			int64_t file_size = 0;
			
		public:
			bool init(string file);
			
			string read(int64_t start_position, int64_t end_position);
	};
	
	class pl_text_display {
		public:
			string data = "";
			
			int rows = 0;
			map<int, int> cols_in_row;
			/*
			cols_in_row:
				First int is the row number
				Second int is the number of columns in that row
				ie,
					Screen:
						ABC
						AB
						A
					cols_in_row[0] == 3
					cols_in_row[1] == 2
					cols_in_row[2] == 1
			*/
			
			void reset();
	};

	class pl_window {
		private:
			bool initialized = false;
			WINDOW* polonius_window;
			
			int height = 0;
			int width = 0;
			int maximum_number_of_chars_on_screen = 0;
			
			vector<screen_position> character_positions;
			/*
			screen_position refers to the X&Y coordinates in the terminal
			The index of the vector is the byte # of the originally-loaded (unediteD) data on screen
			ie,
				Screen:
					012345
					ABCD
				The character 'C' is byte #9 in that data
					(Counting from zero)
					(The 6 digits 0-5 + newline char + 'A' + 'B' coming before it)
				And is at screen position 1,2
					(Y = 1, X = 2)
					(0,0 being the top-left)
			This (pseudo-)map tells us which byte # is at each screen position
			*/
			
			void raw_move_cursor(int y, int x);
			
			pl_text_display attached_text_display;
		public:
			bool is_initialized();
			
			static void handle_force_quit(sig_atomic_t signal);
			static void handle_suspend(sig_atomic_t signal);
			static void handle_resume(sig_atomic_t signal);
			
			static void setup_terminal();
			void resize();
			
			void init(string file_path);
			void write_from_file();
			void refresh_window();
			static void close();
			
			cursor attached_cursor;
			pl_file attached_file;
			
			void move_cursor(movement_plane dimension, int how_far);
			
			void put_char(int ch, bool and_move_cursor = true);
			void put_string(string &input, bool and_move_cursor = true);
			
			void handle_updates();
			
			int get_height();
			int get_width();
	};

}
