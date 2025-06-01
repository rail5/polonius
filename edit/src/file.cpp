/**
 * Copyright (C) 2023-2025 rail5
 */

#include "editor.h"

#include "../../shared/explode.h"
#include "../../shared/to_lower.h"
#include "../../shared/is_number.h"

#include <fstream>

Polonius::Editor::File::File(const std::filesystem::path& filePath) {
	path = filePath;
	if (std::filesystem::exists(path)) {
		size = std::filesystem::file_size(path);
		// Verify that we have write permissions
		if ((std::filesystem::status(path).permissions() & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
			throw std::runtime_error("File is not writable: " + path.string());
		}
	} else {
		// Verify that we have write permissions to the directory
		auto parentPath = path.parent_path();
		if (!std::filesystem::exists(parentPath) || 
		    (std::filesystem::status(parentPath).permissions() & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
			throw std::runtime_error("Directory is not writable: " + parentPath.string());
		}
		// Create the file if it does not exist
		std::ofstream file(path);
		if (!file) {
			throw std::runtime_error("Failed to create file: " + path.string());
		}
		size = 0; // Size is zero for a newly created file
	}
}

void Polonius::Editor::File::parseInstructions(const std::string& instructions) {
	// First, split the instructions string by newlines
	std::vector<std::string> lines = explode(instructions, '\n');

	for (const auto& line : lines) {
		if (line.empty()) {
			continue;
		}

		// Split the line by unescaped semicolons
		std::vector<std::string> parts = explode(line, ';', true);

		// Take the first part
		if (parts.empty()) {
			continue;
		}

		std::vector<std::string> first_part = explode(parts[0], ' ', false, 3);
		if (first_part.size() < 3) {
			throw std::invalid_argument("Invalid instruction format: " + parts[0]);
		}

		InstructionType type;
		// Case-insensitive comparison for the operation type
		std::string op = to_lower(first_part[0]);
		if (op == "insert") {
			type = INSERT;
		} else if (op == "remove") {
			type = REMOVE;
		} else if (op == "replace") {
			type = REPLACE;
		} else {
			throw std::invalid_argument("Unknown instruction type: " + first_part[0]);
		}

		// Verify the position is a number
		if (!is_number(first_part[1])) {
			throw std::invalid_argument("Invalid start position: " + first_part[1]);
		}
		uint64_t start_position = std::stoull(first_part[1]);

		if (type == INSERT || type == REPLACE) {
			std::string text = first_part[2];
			this->instructions.emplace_back(type, start_position, text);
		} else if (type == REMOVE) {
			if (!is_number(first_part[2])) {
				throw std::invalid_argument("Invalid end position for remove operation: " + first_part[2]);
			}
			uint64_t end_position = std::stoull(first_part[2]);
			this->instructions.emplace_back(type, start_position, end_position);
		}

		for (size_t i = 1; i < parts.size(); ++i) {
			std::vector<std::string> additional_parts = explode(parts[i], ' ', false, 2);
			if (additional_parts.size() < 2) {
				throw std::invalid_argument("Invalid instruction format: " + parts[i]);
			}
			// Verify the position is a number
			if (!is_number(additional_parts[0])) {
				throw std::invalid_argument("Invalid start position: " + additional_parts[0]);
			}
			uint64_t additional_start_position = std::stoull(additional_parts[0]);
			if (type == INSERT || type == REPLACE) {
				std::string additional_text = additional_parts[1];
				this->instructions.emplace_back(type, additional_start_position, additional_text);
			} else if (type == REMOVE) {
				if (!is_number(additional_parts[1])) {
					throw std::invalid_argument("Invalid end position for remove operation: " + additional_parts[1]);
				}
				uint64_t additional_end_position = std::stoull(additional_parts[1]);
				this->instructions.emplace_back(type, additional_start_position, additional_end_position);
			}
		}
	}
}

void Polonius::Editor::File::insert(uint64_t position, const std::string& text) {
	if (position > size) {
		throw std::out_of_range("Position is out of bounds");
	}

	std::ofstream file(path, std::ios::in | std::ios::out);
	if (!file) {
		throw std::runtime_error("Failed to open file: " + path.string());
	}

	file.seekp(static_cast<std::streamoff>(position));
	file.write(text.c_str(), static_cast<std::streamsize>(text.size()));
	if (!file) {
		throw std::runtime_error("Failed to write to file: " + path.string());
	}

	size += text.size();
}
