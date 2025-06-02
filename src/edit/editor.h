/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_EDIT_EDITOR_H_
#define SRC_EDIT_EDITOR_H_

#include <filesystem>
#include <string>
#include <stdexcept>
#include <cstdint>
#include <deque>

#include <sys/file.h>

#include "../polonius.h"

namespace Polonius {

class File;

namespace Editor {

extern bool append_newline; // Whether to append a newline to the end of the file after executing instructions

class Instruction;

enum InstructionType : uint8_t {
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

} // namespace Editor
} // namespace Polonius

#endif // SRC_EDIT_EDITOR_H_
