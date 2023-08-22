using namespace std;

namespace reader {

	enum job_type {
		read_job,
		search_job
	};

	enum search_type {
		normal_search,
		regex_search
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

			int64_t end_position = -1;
			
			int block_size = 10240;
			
			string search_query = "";
			search_type query_type = normal_search;
			
			job_type job = read_job;

			void do_read_job();
			void do_search_job();

			void do_normal_search();
			void do_regex_search();
		
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
			void set_search_type(search_type normal_or_regex);
			void set_job_type(job_type input_job);
	};
}
