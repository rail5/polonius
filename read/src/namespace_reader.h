using namespace std;

namespace reader {
	class file {
	
		private:
			bool initialized = false;
			string file_name;
			string init_error_message = "unknown";
			int64_t file_length = 0;
		
		public:
			bool init(string path);
			string read(int64_t start_position, int64_t amount_to_read);
			string get_init_error_message();
			int64_t get_file_length();
	};
}
