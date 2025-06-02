/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_POLONIUS_H_
#define SRC_POLONIUS_H_

#include <cstdint>
#include <deque>
#include <filesystem>

#include "edit/editor.h"

namespace Polonius {

// Forward declarations
namespace Editor {

class Instruction;
enum InstructionType : uint8_t;

}

extern uint64_t block_size;

class File {
	private:
		std::filesystem::path path;
		FILE* file = nullptr;
		int fd = -1; // File descriptor for the file
		uint64_t size = 0;
		bool edits_made = false; // Flag to indicate if any edits were made
		std::deque<Polonius::Editor::Instruction> instructions;

		void insert(uint64_t position, const std::string& text);
		void replace(uint64_t position, const std::string& text);
		void remove(uint64_t start, uint64_t end);

		void validateInstructions() const;

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
};

} // namespace Polonius

#endif // SRC_POLONIUS_H_
