/**
 * Copyright (C) 2024 rail5
 * This is free software (GNU GPL v3)
 * Please refer to the LICENSE file for more information
 */

#include <string>
#include <stdexcept>
#include <array>

class PoloniusHook {
	private:
		std::string reader_path = "polonius-reader";
		std::string editor_path = "polonius-editor";
		std::string file_path = "";
		uint64_t block_size = 102400;

	public:
		explicit PoloniusHook(std::string file_path) {
			this->file_path = file_path;
		}

		std::string read_file(uint64_t start_position = 0, uint64_t length = 0) {
			std::string command = this->reader_path + " " + this->file_path + " -b " + std::to_string(this->block_size);

			if (start_position > 0) {
				command += " -s " + std::to_string(start_position);
			}

			if (length > 0) {
				command += " -l " + std::to_string(length);
			}

			std::array<char, 128> buffer;
			std::string result;

			FILE* pipe = popen(command.c_str(), "r");

			if (!pipe) {
				throw std::runtime_error("Could not open pipe");
			}

			while (fgets(buffer.data(), buffer.size(), pipe) != nullptr) {
				result += buffer.data();
			}

			auto return_code = pclose(pipe);

			if (return_code != 0) {
				throw std::runtime_error("Could not read file");
			}

			return result;
		}
};
