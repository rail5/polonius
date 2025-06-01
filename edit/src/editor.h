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

namespace Polonius {
namespace Editor {

// Forward declarations
class File;
class Instruction;

class File {
	private:
		std::filesystem::path path;
		uint64_t size = 0;
		std::deque<Instruction> instructions;

	public:
		File() = default;
		explicit File(const std::filesystem::path& filePath);

		void parseInstructions(const std::string& instructions);
		std::deque<Instruction> get_instructions() const {
			return instructions;
		}

		void insert(uint64_t position, const std::string& text);
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
