/**
 * Copyright (C) 2025 rail5
 */

#include "polonius.h"
#include "block.h"
#include "shared/process_special_chars.h"

#include <stdexcept>

Polonius::Block::Block(const Polonius::Block& other) {
	this->start_position = other.start_position;
	this->contents = other.contents;
	this->op = other.op;
	this->is_initialized = other.is_initialized;
}

Polonius::Block::Block(Polonius::Block&& other) noexcept {
	this->start_position = other.start_position;
	this->contents = std::move(other.contents);
	this->op = other.op;
	this->is_initialized = other.is_initialized;
	other.contents.clear();
}

Polonius::Block& Polonius::Block::operator=(Polonius::Block&& other) noexcept {
	if (this != &other) {
		this->start_position = other.start_position;
		this->contents = std::move(other.contents);
		this->op = other.op;
		this->is_initialized = other.is_initialized;
		other.contents.clear();
	}
	return *this;
}

std::string Polonius::Block::get_contents() const {
	return contents;
}

void Polonius::Block::set_operator(Polonius::InstructionType op) {
	this->op = op;
}

Polonius::InstructionType Polonius::Block::get_operator() const {
	return op;
}

uint64_t Polonius::Block::size() const {
	return contents.size();
}

uint64_t Polonius::Block::start() const {
	return start_position;
}

uint64_t Polonius::Block::end() const {
	if (contents.empty()) {
		return 0;
	}
	return start_position + contents.size() - 1;
}

char Polonius::Block::at(uint64_t index) const {
	if (index < start_position || index >= start_position + contents.size()) {
		return 0;
	}
	return contents[index - start_position];
}

void Polonius::Block::add(uint64_t start_position, const std::string& value) {
	contents.clear();
	this->start_position = start_position;
	contents = value;
}

void Polonius::Block::add(uint64_t start_position, uint64_t end_position) {
	this->start_position = start_position;
	contents.clear();
	for (uint64_t i = start_position; i <= end_position; ++i) {
		contents += '\0';
	}
}

/**
 * @brief Removes elements in the specified range
 * 
 * This function removes the specified range and then shifts all subsequent elements to the left. * 
 * For example, if the block contains:
 * 		0	1	2	3	4
 * 		a	b	c	d	e
 * And we remove the range 1 to 3, the block will contain:
 * 		0	1
 * 		a	e
 */
void Polonius::Block::remove(uint64_t start_position, uint64_t end_position) {
	if (empty()) {
		return;
	}
	uint64_t removeStart = std::max(start_position, this->start_position);
	uint64_t removeEnd = std::min(end_position, this->end());

	std::string left_hand_side = contents.substr(0, removeStart - this->start_position);
	std::string right_hand_side = contents.substr(removeEnd - this->start_position + 1);
	contents = left_hand_side + right_hand_side;
	
	// Update the start position if we removed from the beginning
	if (removeStart <= this->start_position) {
		this->start_position = removeEnd + 1;
	}
}

/**
 * @brief Clear the entire block (set it to an empty block)
 */
void Polonius::Block::clear() {
	start_position = 0;
	contents.clear();
}

bool Polonius::Block::shift_left(uint64_t shift_amount) {
	if (contents.empty()) {
		return false;
	}

	// Reject if the shift would cause the start position to go negative
	if (shift_amount > start()) {
		return false;
	}

	start_position -= shift_amount;
	
	return true;
}

bool Polonius::Block::shift_right(uint64_t shift_amount) {
	if (contents.empty()) {
		return false;
	}

	// Shift the data
	start_position += shift_amount;
	
	return true;
}

bool Polonius::Block::empty() const {
	return contents.empty();
}

void Polonius::Block::set_initialized(bool initialized) {
	is_initialized = initialized;
}

bool Polonius::Block::initialized() const {
	return is_initialized;
}

/**
 * @brief Calculates the overlap of two blocks.
 * 
 * For example:
 * Block A:
 * 		0	1	2	3	4
 * 		a	b	c	d	e
 * Block B:
 * 		2	3	4	5
 * 		x	y	z	a
 * They overlap between positions 2 and 4
 * 
 * The first element of the pair is the start of the overlap,
 * and the second element is the end of the overlap.
 * If there is no overlap, both elements will be 0.
 */
Polonius::BlockOverlap Polonius::Block::overlap(const Polonius::Block& b) const {
	Polonius::BlockOverlap result;
	result.empty = true;

	if (empty() || b.empty()) {
		return result;
	}

	if (start() <= b.end() && end() >= b.start()) {
		result.start = std::max(start(), b.start());
		result.end = std::min(end(), b.end());
		result.empty = false;
	}

	return result;
}

/**
 * @brief Calculates the overlap of this block with a specified range.
 * 
 * Just like the previous function, but instead of pulling the range from another block,
 * it takes a start and end position as parameters.
 * 
 * For example:
 * If this block is:
 * 		5	6	7	8	9
 * 		a	b	c	d	e
 * And the range is 0 to 7,
 * the overlap will be between positions 5 and 7.
 */
Polonius::BlockOverlap Polonius::Block::overlap(uint64_t start_position, uint64_t end_position) const {
	Polonius::BlockOverlap result;
	result.empty = true;
	if (empty()) {
		return result;
	}

	if (start() <= end_position && end() >= start_position) {
		result.start = std::max(start(), start_position);
		result.end = std::min(end(), end_position);
		result.empty = false;
	}
	return result;
}

namespace Polonius {
Polonius::Block combine_inserts(const Polonius::Block& lhs, const Polonius::Block& rhs) {
	// Verify they're both inserts
	if (lhs.get_operator() != Polonius::InstructionType::INSERT || rhs.get_operator() != Polonius::InstructionType::INSERT) {
		throw std::invalid_argument("Both blocks must be INSERT operations to combine.");
	}

	Polonius::Block combined;

	if (lhs.empty() || rhs.empty()) {
		return combined; // If either block is empty, return empty block
	}

	// Verify they overlap
	Polonius::BlockOverlap overlap = lhs.overlap(rhs);
	if (overlap.empty || lhs.start() > rhs.start()) {
		return combined; // No overlap, return empty block
	}

	combined.set_operator(Polonius::InstructionType::INSERT);
	
	std::string first, second, third;
	size_t first_offset, third_offset;

	first = lhs.start() <= rhs.start() ? lhs.get_contents() : rhs.get_contents();
	first_offset = lhs.start() <= rhs.start() ? lhs.start() : rhs.start();
	first = first.substr(0, overlap.start - first_offset);

	second = lhs.start() <= rhs.start() ? rhs.get_contents() : lhs.get_contents();

	third = lhs.start() <= rhs.start() ? lhs.get_contents() : rhs.get_contents();
	third_offset = lhs.start() <= rhs.start() ? lhs.start() : rhs.start();
	third = third.substr(overlap.start - third_offset);

	combined.add(lhs.start(), first + second + third);
	return combined;
}

Polonius::Block combine_removes(const Polonius::Block& lhs, const Polonius::Block& rhs) {
	// Verify they're both removes
	if (lhs.get_operator() != Polonius::InstructionType::REMOVE || rhs.get_operator() != Polonius::InstructionType::REMOVE) {
		throw std::invalid_argument("Both blocks must be REMOVE operations to combine.");
	}

	Polonius::Block combined;

	if (lhs.empty() || rhs.empty()) {
		return combined; // If either block is empty, return empty block
	}


	// Verify they overlap in precisely the following way:
	// The START position of the lhs is BETWEEN the START and END positions of the rhs
	if (lhs.start() < rhs.start() || lhs.start() > rhs.end()) {
		return combined; // No overlap of this kind, return empty block
	}

	combined.set_operator(Polonius::InstructionType::REMOVE);
	uint64_t combined_end = rhs.end() + lhs.size();
	combined.add(rhs.start(), combined_end);

	return combined;
}
} // namespace Polonius
