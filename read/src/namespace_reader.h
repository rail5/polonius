using namespace std;

namespace reader {
	class file {
	
		private:
			bool initialized = false;
			string file_name;
			string init_error_message = "unknown";
			int64_t file_length = 0;
			
			bool just_outputting_positions = false;
			int64_t start_position = 0;
			int64_t amount_to_read = -1;
			
			int block_size = 1024;
		
		public:
			bool init(string path);
			string read();
			string search(string query);
			string get_init_error_message();
			int64_t get_file_length();
			
			void set_start_position(int64_t position);
			void set_amount_to_read(int64_t amount);
			void set_just_outputting_positions(bool flag);
	};
}
