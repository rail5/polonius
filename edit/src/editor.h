/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef EDIT_SRC_EDITOR_H_
#define EDIT_SRC_EDITOR_H_

#include <filesystem>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <deque>

#include <sys/file.h>

namespace Polonius {
namespace Editor {

static uint64_t block_size = 10240;

// Forward declarations
class File;
class Instruction;

class File {
	private:
		std::filesystem::path path;
		FILE* file = nullptr;
		int fd = -1; // File descriptor for the file
		uint64_t size = 0;
		std::deque<Instruction> instructions;

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
		std::deque<Instruction> get_instructions() const {
			return instructions;
		}

		void executeInstructions();
};

enum InstructionType {
	INSERT,
	REMOVE,
	REPLACE
};

class Instruction {
	private:
		InstructionType op;
		uint64_t start_position;
		uint64_t end_position;
		std::string text;
	public:
		Instruction() = default;
		Instruction(InstructionType type, uint64_t start, const std::string& text);
		Instruction(InstructionType type, uint64_t start, uint64_t end);
		InstructionType type() const;
		uint64_t start() const;
		uint64_t end() const;
		uint64_t size() const;
		const std::string& get_text() const;

		void set_type(InstructionType type);
		void set_start(uint64_t start);
		void set_end(uint64_t end);
		void set_text(const std::string& text);
};

Instruction parse(const std::string& instruction);

} // namespace Editor
} // namespace Polonius

#endif // EDIT_SRC_EDITOR_H_
