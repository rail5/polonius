using namespace std;

namespace reader {
	class file {
	
		private:
			bool initialized = false;
			string file_name;
			string init_error_message = "unknown";
			long long int file_length = 0;
		
		public:
			bool init(string path);
			string read(long long int start_position, long long int amount_to_read);
			string get_init_error_message();
			long long int get_file_length();
	};
}
