/**
 * Copyright (C) 2025 rail5
 */

#ifndef SRC_FILE_H_
#define SRC_FILE_H_

#include "polonius.h"
#include "edit/expression.h"

namespace Polonius {
class File {
	private:
		std::filesystem::path path;
		FILE* file = nullptr;
		int fd = -1; // File descriptor for the file
		bool new_file = false; // Whether the file was created by this instance
		bool instructions_valid = true; // Whether the instructions are valid
		uint64_t size = 0;
		Polonius::Editor::Expression instructions;

		void insert(uint64_t position, const std::string& text);
		void replace(uint64_t position, const std::string& text);
		void remove(uint64_t start, uint64_t end);

		bool ends_with_newline() const;

		void validateInstructions();

	public:
		File() = default;
		explicit File(const std::filesystem::path& filePath);
		~File();
		File(const File&) = delete; // Disable copy constructor
		File& operator=(const File&) = delete; // Disable copy assignment operator
		File(File&& other) noexcept; // Move constructor
		File& operator=(File&& other) noexcept; // Move assignment operator
		
		std::string getPath() const;
		uint64_t getSize() const;

		void parseInstructions(const std::string& instructions);
		void executeInstructions();

		void set_optimization_level(uint8_t level);

		Polonius::Block search(uint64_t start, int64_t length, const std::string& query) const;
		Polonius::Block search_backwards(uint64_t start, int64_t length, const std::string& query) const;
		Polonius::Block regex_search(uint64_t start, int64_t length, const std::string& query) const;

		Polonius::Block readFromFile(uint64_t start = Polonius::Reader::start_position,
			int64_t length = Polonius::Reader::amount_to_read,
			bool to_next_newline = false) const;
		
		Polonius::Block readLines(uint64_t start, int number_of_lines, bool stop_at_blocksize = false) const;
		Polonius::Block readLines_backwards(uint64_t start, int number_of_lines, bool stop_at_blocksize = false) const;
};

} // namespace Polonius

#endif // SRC_FILE_H_
