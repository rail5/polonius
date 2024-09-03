/***
 * Copyright (C) 2024 rail5
*/

#ifndef READ_SRC_NAMESPACE_READER_H_
#define READ_SRC_NAMESPACE_READER_H_

namespace reader {

enum job_type {
	read_job,
	search_job
};

enum search_type {
	t_normal_search,
	t_regex_search
};

class file {
	private:
		bool initialized = false;
		std::string file_name;
		std::string init_error_message = "unknown";
		int64_t file_length = 0;
		
		bool just_outputting_positions = false;
		int64_t start_position = 0;
		int64_t amount_to_read = -1;

		int64_t end_position = -1;
		
		int64_t block_size = 10240;
		
		std::string search_query = "";
		search_type query_type = t_normal_search;
		
		job_type job = read_job;

		bool do_read_job();
		bool do_search_job();

		bool do_normal_search();
		bool do_regex_search();
	
	public:
		bool init(std::string path);
		
		std::string read(int64_t start_position, int64_t length);
		
		std::string get_init_error_message();
		int64_t get_file_length();
		
		bool do_job();
		
		void set_start_position(int64_t position);
		void set_amount_to_read(int64_t amount);
		void set_just_outputting_positions(bool flag);
		void set_block_size(int64_t size);
		void set_search_query(std::string query);
		void set_search_type(search_type normal_or_regex);
		void set_job_type(job_type input_job);
};
} // namespace reader

#endif // READ_SRC_NAMESPACE_READER_H_
