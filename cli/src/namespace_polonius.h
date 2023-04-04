#ifndef CURSES_H
	#define CURSES_H
	#include <curses.h>
#endif

#ifndef ARRAY
	#define ARRAY
	#include <array>
#endif

using namespace std;

namespace polonius {
	
	/*
	Forward declarations of classes:
	*/
	class cursor;
	class command;
	class pl_file;
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
			int y_limit = 0;
			int x_limit = 0;
		public:
			void set_initialized(bool set_value);
			
			void move(int y, int x);
			void set_limits(int y, int x);
			
			int get_y_coordinate();
			int get_x_coordinate();
			
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
				
				exit_code = WEXITSTATUS(pclose(pipe));
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

	class pl_window {
		private:
			bool initialized = false;
			WINDOW* polonius_window;
			
			int height = 0;
			int width = 0;
			int maximum_number_of_chars_on_screen = 0;
			
			void raw_move_cursor(int y, int x);
		public:
			bool is_initialized();
		
			void init(string file_path);
			void write_from_file();
			void refresh_window();
			void close();
			
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
