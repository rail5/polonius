using namespace std;

namespace reader {

	enum job_type {
		read_job,
		search_job
	};

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
			
			string search_query = "";
			
			job_type job = read_job;
		
		public:
			bool init(string path);
			
			string read(int64_t start_position, int64_t length);
			string search(string query);
			
			string get_init_error_message();
			int64_t get_file_length();
			
			void do_job();
			
			void set_start_position(int64_t position);
			void set_amount_to_read(int64_t amount);
			void set_just_outputting_positions(bool flag);
			void set_block_size(int size);
			void set_search_query(string query);
			void set_job_type(job_type input_job);
	};
}
