/**
 * Copyright (C) 2023-2025 rail5
 */

#include "editor.h"

Polonius::Editor::Instruction::Instruction(InstructionType type, uint64_t start, const std::string& text) {
	op = type;
	start_position = start;
	end_position = start + text.size();
	this->text = text;
}

Polonius::Editor::Instruction::Instruction(InstructionType type, uint64_t start, uint64_t end) {
	if (type != REMOVE) {
		throw std::invalid_argument("Only REMOVE operations can have a range without text");
	}
	op = REMOVE;
	start_position = start;
	end_position = end;
	text.clear();
}

Polonius::Editor::InstructionType Polonius::Editor::Instruction::type() const {
	return op;
}

uint64_t Polonius::Editor::Instruction::start() const {
	return start_position;
}

uint64_t Polonius::Editor::Instruction::end() const {
	return end_position;
}

const std::string& Polonius::Editor::Instruction::get_text() const {
	return text;
}

void Polonius::Editor::Instruction::set_type(InstructionType type) {
	op = type;
}

void Polonius::Editor::Instruction::set_start(uint64_t start) {
	start_position = start;
}

void Polonius::Editor::Instruction::set_end(uint64_t end) {
	end_position = end;
}

void Polonius::Editor::Instruction::set_text(const std::string& text) {
	this->text = text;
	end_position = start_position + text.size();
}
