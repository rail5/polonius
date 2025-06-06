/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_POLONIUS_H_
#define SRC_POLONIUS_H_

#include <cstdint>
#include <deque>
#include <filesystem>

#include "edit/editor.h"
#include "read/reader.h"

namespace Polonius {

// Forward declarations
namespace Editor {
class Instruction;
enum InstructionType : uint8_t;
}

namespace Reader {
enum search_type : uint8_t;
}

extern uint8_t exit_code; // Exit code for the program
extern uint64_t block_size;
extern bool editor_mode;
extern bool reader_mode;
extern bool special_chars; // Whether to process escape sequences in strings

struct Block {
	uint64_t start = 0;
	uint64_t end() const {
		if (!initialized || contents.empty()) {
			return 0; // If not initialized, return 0
		}
		return start + contents.size() - 1; // -1 because the end is inclusive
	}
	std::string contents = "";
	bool initialized = false;
};

class File {
	private:
		std::filesystem::path path;
		FILE* file = nullptr;
		int fd = -1; // File descriptor for the file
		bool new_file = false; // Whether the file was created by this instance
		bool instructions_valid = true; // Whether the instructions are valid
		uint64_t size = 0;
		std::deque<Polonius::Editor::Instruction> instructions;

		std::string search_query;

		void insert(uint64_t position, const std::string& text);
		void replace(uint64_t position, const std::string& text);
		void remove(uint64_t start, uint64_t end);

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

		void parseInstructions(const std::string& instructions);
		void executeInstructions();

		void setSearchQuery(const std::string& query);

		Polonius::Block search(uint64_t start, int64_t length, const std::string& query) const;
		Polonius::Block regex_search(uint64_t start, int64_t length, const std::string& query) const;

		Polonius::Block readFromFile(uint64_t start = Polonius::Reader::start_position,
			int64_t length = Polonius::Reader::amount_to_read,
			bool to_nearest_newline = false) const;
		Polonius::Block read();
};

} // namespace Polonius

#endif // SRC_POLONIUS_H_
