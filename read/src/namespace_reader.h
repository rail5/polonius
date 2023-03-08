using namespace std;

namespace reader {
	class file {
	
		private:
			bool initialized = false;
			string file_name;
			string init_error_message = "unknown";
			int file_length = 0;
		
		public:
			bool init(string path);
			string read(int start_position, int amount_to_read);
			string get_init_error_message();
			int get_file_length();
	};
}
