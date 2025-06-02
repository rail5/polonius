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

extern uint64_t block_size;
extern bool editor_mode;
extern bool reader_mode;

class File {
	private:
		std::filesystem::path path;
		FILE* file = nullptr;
		int fd = -1; // File descriptor for the file
		uint64_t size = 0;
		std::deque<Polonius::Editor::Instruction> instructions;

		std::string search_query;

		void insert(uint64_t position, const std::string& text);
		void replace(uint64_t position, const std::string& text);
		void remove(uint64_t start, uint64_t end);

		void validateInstructions() const;

		std::string readFromFile(uint64_t start = Polonius::Reader::start_position,
			int64_t length = Polonius::Reader::amount_to_read,
			bool force_output_text = false) const;
		void search() const;
		void regex_search() const;

	public:
		File() = default;
		explicit File(const std::filesystem::path& filePath);
		~File();
		File(const File&) = delete; // Disable copy constructor
		File& operator=(const File&) = delete; // Disable copy assignment operator
		File(File&& other) noexcept; // Move constructor
		File& operator=(File&& other) noexcept; // Move assignment operator

		void parseInstructions(const std::string& instructions);
		void executeInstructions();

		void setSearchQuery(const std::string& query);

		void read() const;
};

} // namespace Polonius

#endif // SRC_POLONIUS_H_
