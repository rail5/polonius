/**
 * Copyright (C) 2023-2025 rail5
 */

#include "polonius.h"

#include "shared/explode.h"
#include "shared/to_lower.h"
#include "shared/is_number.h"

#include <vector>
#include <fstream>
#include <unistd.h>

uint64_t Polonius::block_size = 10240; // Default block size is 10K
bool Polonius::Editor::append_newline = true; // Default is to append a newline at the end of the file

Polonius::File::File(const std::filesystem::path& filePath) {
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

	file = fopen64(path.string().c_str(), "r+b");
	if (!file) {
		throw std::runtime_error("Failed to open file: " + path.string());
	}
	fd = fileno(file);
	if (fd < 0) {
		fclose(file);
		throw std::runtime_error("Failed to get file descriptor for: " + path.string());
	}

	// Lock the file to prevent concurrent modifications
	if (flock(fd, LOCK_EX | LOCK_NB) < 0) {
		fclose(file);
		throw std::runtime_error("Failed to lock file: " + path.string());
	}
}

Polonius::File::~File() {
	if (file) {
		// Unlock the file before closing
		if (fd >= 0) {
			flock(fd, LOCK_UN);
		}
		fclose(file);
	}
}

Polonius::File::File(File&& other) noexcept {
	path = std::move(other.path);
	file = other.file;
	fd = other.fd;
	size = other.size;
	instructions = std::move(other.instructions);

	// Reset the moved-from object
	other.file = nullptr;
	other.fd = -1;
	other.size = 0;
}

Polonius::File& Polonius::File::operator=(File&& other) noexcept {
	if (this != &other) {
		// Unlock the current file if it is open
		if (file) {
			if (fd >= 0) {
				flock(fd, LOCK_UN);
			}
			fclose(file);
		}

		path = std::move(other.path);
		file = other.file;
		fd = other.fd;
		size = other.size;
		instructions = std::move(other.instructions);

		// Reset the moved-from object
		other.file = nullptr;
		other.fd = -1;
		other.size = 0;
	}

	return *this;
}

void Polonius::File::validateInstructions() const {
	// Keep a running tally of the size of the file
	uint64_t size_after_last_instruction = size;
	for (const auto& instruction : instructions) {
		switch (instruction.type()) {
			case Polonius::Editor::INSERT:
				if (instruction.start() > size_after_last_instruction) {
					throw std::out_of_range("Insert position is out of bounds: " + std::to_string(instruction.start()));
				}
				size_after_last_instruction += instruction.size();
				break;
			case Polonius::Editor::REMOVE:
				if (instruction.end() > size_after_last_instruction || size_after_last_instruction == 0) {
					throw std::out_of_range("Remove position is out of bounds: " + std::to_string(instruction.start()) + " - " + std::to_string(instruction.end()));
				}
				if (instruction.start() > instruction.end()) {
					throw std::invalid_argument("Remove start position is greater than end position: " + std::to_string(instruction.start()) + " - " + std::to_string(instruction.end()));
				}
				size_after_last_instruction -= instruction.size();
				break;
			case Polonius::Editor::REPLACE:
				if (instruction.start() > size_after_last_instruction || instruction.start() + instruction.size() > size_after_last_instruction) {
					throw std::out_of_range("Replace position is out of bounds: " + std::to_string(instruction.start()));
				}
				break; // REPLACE instructions do not change the size of the file
		}
	}
}

void Polonius::File::parseInstructions(const std::string& instructions) {
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

		Polonius::Editor::InstructionType type;
		// Case-insensitive comparison for the operation type
		std::string op = to_lower(first_part[0]);
		if (op == "insert") {
			type = Polonius::Editor::INSERT;
		} else if (op == "remove") {
			type = Polonius::Editor::REMOVE;
		} else if (op == "replace") {
			type = Polonius::Editor::REPLACE;
		} else {
			throw std::invalid_argument("Unknown instruction type: " + first_part[0]);
		}

		// Verify the position is a number
		if (!is_number(first_part[1])) {
			throw std::invalid_argument("Invalid start position: " + first_part[1]);
		}
		uint64_t start_position = std::stoull(first_part[1]);

		if (type == Polonius::Editor::INSERT || type == Polonius::Editor::REPLACE) {
			std::string text = first_part[2];
			this->instructions.emplace_back(type, start_position, text);
		} else if (type == Polonius::Editor::REMOVE) {
			if (!is_number(first_part[2])) {
				throw std::invalid_argument("Invalid end position for remove operation: " + first_part[2]);
			}
			uint64_t end_position = std::stoull(first_part[2]);
			this->instructions.emplace_back(type, start_position, end_position);
		}

		for (size_t i = 1; i < parts.size(); ++i) {
			// Trim leading whitespace from parts[i]
			size_t first_non_space = parts[i].find_first_not_of(' ');
			if (first_non_space != std::string::npos) {
				parts[i].erase(0, first_non_space);
			} else {
				continue; // Skip empty parts
			}
			std::vector<std::string> additional_parts = explode(parts[i], ' ', false, 2);
			if (additional_parts.size() < 2) {
				throw std::invalid_argument("Invalid instruction format: " + parts[i]);
			}
			// Verify the position is a number
			if (!is_number(additional_parts[0])) {
				throw std::invalid_argument("Invalid start position: " + additional_parts[0]);
			}
			uint64_t additional_start_position = std::stoull(additional_parts[0]);
			if (type == Polonius::Editor::INSERT || type == Polonius::Editor::REPLACE) {
				std::string additional_text = additional_parts[1];
				this->instructions.emplace_back(type, additional_start_position, additional_text);
			} else if (type == Polonius::Editor::REMOVE) {
				if (!is_number(additional_parts[1])) {
					throw std::invalid_argument("Invalid end position for remove operation: " + additional_parts[1]);
				}
				uint64_t additional_end_position = std::stoull(additional_parts[1]);
				this->instructions.emplace_back(type, additional_start_position, additional_end_position);
			}
		}
	}
}

void Polonius::File::insert(uint64_t position, const std::string& text) {
	if (position > size) {
		throw std::out_of_range("Position is out of bounds");
	}

	size_t original_size = size;
	size = size + text.size();

	// Add blank space to the end of the file to make room for the new text
	std::filesystem::resize_file(path, size);

	// Right-shift the existing content to make room for the new text
	// Copy block_size bytes from position to the end of the file
	// Moving backwards until we hit the position we want to insert to
	// And then finally, replace at that position with the new text
	// I.e.:
	// We're keeping 2 pointers:
	// 1. To the old end of the file minus the block size
	// 2. To the new end of the file minus the block size
	// And we're continuously copying text from the old end to the new end
	// While walking both pointers backwards by the block size
	// Until we hit the position we want to insert to
	uint64_t old_pointer = original_size;
	uint64_t new_pointer = size;

	while (old_pointer > position) {
		uint64_t bytes_to_copy = std::min(Polonius::block_size, old_pointer - position);
		std::vector<char> buffer(bytes_to_copy);

		fseeko64(file, static_cast<int64_t>(old_pointer - bytes_to_copy), SEEK_SET);
		fread(buffer.data(), 1, bytes_to_copy, file);

		fseeko64(file, static_cast<int64_t>(new_pointer - bytes_to_copy), SEEK_SET);
		fwrite(buffer.data(), 1, bytes_to_copy, file);

		fflush(file);

		new_pointer -= bytes_to_copy;
		old_pointer -= bytes_to_copy;
	}

	// Now we can write the new text at the position
	fseeko64(file, static_cast<int64_t>(position), SEEK_SET);
	if (fwrite(text.data(), 1, text.size(), file) != text.size()) {
		throw std::runtime_error("Failed to write text to file at position " + std::to_string(position));
	}
	fflush(file); // Ensure the data is written to disk
}

void Polonius::File::remove(uint64_t start, uint64_t end) {
	if (start > size || end > size || start > end) {
		throw std::out_of_range("Start or end position is out of bounds");
	}

	size_t bytes_to_remove = end - start + 1;
	size_t original_size = size;
	size = size - bytes_to_remove;

	// Left-shift the content after the removed section
	// Do this in chunks of block_size bytes
	// And then truncate the file to the new size
	size_t old_pos = end + 1; // Position after the removed section
	size_t new_pos = start;
	while (old_pos < original_size) {
		size_t bytes_to_copy = std::min(Polonius::block_size, original_size - old_pos);
		std::vector<char> buffer(bytes_to_copy);

		fseeko64(file, static_cast<int64_t>(old_pos), SEEK_SET);
		if (fread(buffer.data(), 1, bytes_to_copy, file) != bytes_to_copy) {
			throw std::runtime_error("Failed to read from file at position " + std::to_string(old_pos));
		}
		fseeko64(file, static_cast<int64_t>(new_pos), SEEK_SET);
		if (fwrite(buffer.data(), 1, bytes_to_copy, file) != bytes_to_copy) {
			throw std::runtime_error("Failed to write to file at position " + std::to_string(new_pos));
		}
		fflush(file); // Ensure the data is written to disk

		new_pos += bytes_to_copy;
		old_pos += bytes_to_copy;
	}
	// Truncate the file to the new size
	std::filesystem::resize_file(path, size);
}

void Polonius::File::replace(uint64_t position, const std::string& text) {
	if (position > size || position + text.size() > size) {
		throw std::out_of_range("Position is out of bounds");
	}

	// Move the file pointer to the position
	fseeko64(file, static_cast<int64_t>(position), SEEK_SET);
	if (fwrite(text.data(), 1, text.size(), file) != text.size()) {
		throw std::runtime_error("Failed to write text to file at position " + std::to_string(position));
	}
	fflush(file); // Ensure the data is written to disk
}

void Polonius::File::executeInstructions() {
	validateInstructions();
	for (const auto& instruction : instructions) {
		switch (instruction.type()) {
			case Polonius::Editor::INSERT:
				insert(instruction.start(), instruction.get_text());
				break;
			case Polonius::Editor::REMOVE:
				remove(instruction.start(), instruction.end());
				break;
			case Polonius::Editor::REPLACE:
				replace(instruction.start(), instruction.get_text());
				break;
		}
	}
	if (Polonius::Editor::append_newline && !instructions.empty()) {
		// Add a newline to the end of the file if it does not end with one
		if (size > 0 && fseek(file, -1, SEEK_END) == 0) {
			int last_char = fgetc(file);
			if (last_char != '\n') {
				fputc('\n', file);
				size++;
			}
		}
	}
	instructions.clear(); // Clear instructions after execution
}
