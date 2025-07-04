/**
 * Copyright (C) 2023-2025 rail5
 */

#include "polonius.h"
#include "file.h"

#include "shared/explode.h"
#include "shared/to_lower.h"
#include "shared/is_number.h"
#include "shared/parse_regex.h"
#include "shared/process_special_chars.h"

#include <vector>
#include <fstream>
#include <unistd.h>

#include <boost/regex.hpp>

// General options:
uint64_t Polonius::block_size = 10240; // Default block size is 10K
bool Polonius::editor_mode = true;
bool Polonius::reader_mode = true;
bool Polonius::special_chars = false; // Default is to not process special characters

uint8_t Polonius::exit_code = EXIT_SUCCESS; // Default exit code is success

// Editor options:
bool Polonius::Editor::append_newline = true; // Default is to append a newline at the end of the file

// Reader options:
bool Polonius::Reader::output_positions = false; // Default is to output text
Polonius::Reader::search_type Polonius::Reader::search_mode = Polonius::Reader::t_normal_search; // Default search mode is normal search
uint64_t Polonius::Reader::start_position = 0; // Default start position is 0
int64_t Polonius::Reader::amount_to_read = -1; // Default is to read until EOF
std::ostream& Polonius::Reader::output_stream = std::cout; // Default output stream is std::cout

/**
 * @brief Constructs a Polonius::File object.
 * 
 * This constructor initializes the file object by opening the specified file path.
 * 
 * It checks if the file exists and whether it is writable. If the file does not exist and we are in editor mode, it creates the file.
 * If the file does not exist and we are in reader mode, it throws an error.
 * 
 * It also locks the file to prevent concurrent modifications.
 */
Polonius::File::File(const std::filesystem::path& filePath) {
	path = filePath;
	if (std::filesystem::exists(path)) {
		size = std::filesystem::file_size(path);
		
		// If we're in editor mode, we need write permissions on the file
		if (Polonius::editor_mode) {
			// Verify that we have write permissions
			if ((std::filesystem::status(path).permissions() & std::filesystem::perms::owner_write) == std::filesystem::perms::none) {
				throw std::runtime_error("File is not writable: " + path.string());
			}
		}
	} else if (Polonius::editor_mode) {
		// If we're in editor mode and the file does not exist, create it

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
		new_file = true;
		size = 0; // Size is zero for a newly created file
	} else {
		// If we're in reader mode and the file does not exist, throw an error
		// We do not create files in reader mode
		throw std::runtime_error("File does not exist: " + path.string());
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

/**
 * @brief Destructor for Polonius::File.
 * 
 * This destructor closes the file and unlocks it if it is currently open.
 */
Polonius::File::~File() {
	if (file) {
		// Unlock the file before closing
		if (fd >= 0) {
			flock(fd, LOCK_UN);
		}
		fclose(file);
	}

	if (!instructions_valid && new_file) {
		// If the instructions are not valid and the file was created by this instance,
		// remove the file to avoid leaving an invalid file behind
		std::filesystem::remove(path);
	}
}

Polonius::File::File(File&& other) noexcept {
	path = std::move(other.path);
	file = other.file;
	fd = other.fd;
	size = other.size;
	new_file = other.new_file;
	instructions_valid = other.instructions_valid;
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
		new_file = other.new_file;
		instructions_valid = other.instructions_valid;
		instructions = std::move(other.instructions);

		// Reset the moved-from object
		other.file = nullptr;
		other.fd = -1;
		other.size = 0;
	}

	return *this;
}

std::string Polonius::File::getPath() const {
	return path.string();
}

uint64_t Polonius::File::getSize() const {
	return size;
}

bool Polonius::File::ends_with_newline() const {
	if (size == 0) {
		return false; // An empty file does not end with a newline
	}

	// Move the file pointer to the end of the file
	fseek(file, -1, SEEK_END);
	char last_char;
	if (fread(&last_char, 1, 1, file) != 1) {
		throw std::runtime_error("Failed to read last character of file: " + path.string());
	}

	return last_char == '\n';
}

/**
 * @brief Validates the instruction sequence for the file.
 * 
 * This function checks the sequence of instructions to ensure that they are valid and do not exceed the bounds of the file.
 * 
 * If any instruction is invalid, it throws an exception with a descriptive error message.
 */
void Polonius::File::validateInstructions() {
	// Keep a running tally of the size of the file
	uint64_t size_after_last_instruction = size;
	if (ends_with_newline()) {
		size_after_last_instruction--; // If the file ends with a newline, we do not count it in the size
	}
	for (const auto& instruction : instructions) {
		switch (instruction.get_operator()) {
			case Polonius::InstructionType::INSERT:
				if (instruction.start() > size_after_last_instruction) {
					instructions_valid = false;
					throw std::out_of_range("Insert position is out of bounds: " + std::to_string(instruction.start()));
				}
				size_after_last_instruction += instruction.size();
				break;
			case Polonius::InstructionType::REMOVE:
				if (instruction.end() >= size_after_last_instruction || size_after_last_instruction == 0) {
					instructions_valid = false;
					throw std::out_of_range("Remove position is out of bounds: " + std::to_string(instruction.start()) + " - " + std::to_string(instruction.end()));
				}
				if (instruction.start() > instruction.end()) {
					instructions_valid = false;
					throw std::invalid_argument("Remove start position is greater than end position: " + std::to_string(instruction.start()) + " - " + std::to_string(instruction.end()));
				}
				size_after_last_instruction -= instruction.size();
				break;
			case Polonius::InstructionType::REPLACE:
				if (instruction.start() + instruction.size() > size_after_last_instruction) {
					instructions_valid = false;
					throw std::out_of_range("Replace position is out of bounds: " + std::to_string(instruction.start()));
				}
				break; // REPLACE instructions do not change the size of the file
		}
	}
}

/**
 * @brief Parses an instruction sequence from a string.
 * 
 * This function takes a string containing multiple instructions, each separated by newlines,
 * and adds the parsed instructions to the file's instruction list.
 * 
 * The format of the instruction sequence is:
 * ```
 * [INSERT|REMOVE|REPLACE] <start> <text|end>; <start> <text|end> ...
 * ```
 * 
 * For example:
 * 
 * ```
 * INSERT 0 hello; 5 world
 * REPLACE 15 goodbye; 20 farewell
 * REMOVE 10 15; 20 25
 * ```
 */
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
			instructions_valid = false;
			throw std::invalid_argument("Invalid instruction format: " + parts[0]);
		}

		Polonius::InstructionType type;
		// Case-insensitive comparison for the operation type
		std::string op = to_lower(first_part[0]);
		if (op == "insert") {
			type = Polonius::InstructionType::INSERT;
		} else if (op == "remove") {
			type = Polonius::InstructionType::REMOVE;
		} else if (op == "replace") {
			type = Polonius::InstructionType::REPLACE;
		} else {
			instructions_valid = false;
			throw std::invalid_argument("Unknown instruction type: " + first_part[0]);
		}

		// Verify the position is a number
		if (!is_number(first_part[1])) {
			instructions_valid = false;
			throw std::invalid_argument("Invalid start position: " + first_part[1]);
		}
		uint64_t start_position = std::stoull(first_part[1]);

		if (type == Polonius::InstructionType::INSERT || type == Polonius::InstructionType::REPLACE) {
			std::string text = first_part[2];
			this->instructions.add_term(type, start_position, text);
		} else if (type == Polonius::InstructionType::REMOVE) {
			if (!is_number(first_part[2])) {
				instructions_valid = false;
				throw std::invalid_argument("Invalid end position for remove operation: " + first_part[2]);
			}
			uint64_t end_position = std::stoull(first_part[2]);
			this->instructions.add_term(type, start_position, end_position);
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
				instructions_valid = false;
				throw std::invalid_argument("Invalid start position: " + additional_parts[0]);
			}
			uint64_t additional_start_position = std::stoull(additional_parts[0]);
			if (type == Polonius::InstructionType::INSERT || type == Polonius::InstructionType::REPLACE) {
				std::string additional_text = additional_parts[1];
				this->instructions.add_term(type, additional_start_position, additional_text);
			} else if (type == Polonius::InstructionType::REMOVE) {
				if (!is_number(additional_parts[1])) {
					instructions_valid = false;
					throw std::invalid_argument("Invalid end position for remove operation: " + additional_parts[1]);
				}
				uint64_t additional_end_position = std::stoull(additional_parts[1]);
				this->instructions.add_term(type, additional_start_position, additional_end_position);
			}
		}
	}
}

/**
 * @brief Executes an INSERT instruction at the specified position.
 */
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

/**
 * @brief Executes a REMOVE instruction to delete a range of text from the file.
 */
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

/**
 * @brief Executes a REPLACE instruction to replace text at a specific position.
 */
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

/**
 * @brief Executes all instructions in the file's instruction list.
 * 
 * At the end of execution, if the file has been modified and the `append_newline` option is enabled,
 * it appends a newline character to the end of the file if it does not already end with one.
 */
void Polonius::File::executeInstructions() {
	validateInstructions();
	for (const auto& instruction : instructions) {
		switch (instruction.get_operator()) {
			case Polonius::InstructionType::INSERT:
				insert(instruction.start(), instruction.get_contents());
				break;
			case Polonius::InstructionType::REMOVE:
				remove(instruction.start(), instruction.end());
				break;
			case Polonius::InstructionType::REPLACE:
				replace(instruction.start(), instruction.get_contents());
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

void Polonius::File::set_optimization_level(uint8_t level) {
	instructions.set_optimization_level(level);
}

/**
 * @brief Reads data from the file starting from a specific position.
 * 
 * This function reads data from the file starting at the specified position and for the specified length.
 * If the length is negative, it reads until the end of the file.
 * If the end position exceeds the file size, it reads until the end of the file.
 */
Polonius::Block Polonius::File::readFromFile(uint64_t start, int64_t length, bool to_nearest_newline) const {
	Polonius::Block result;
	if (!file) {
		return result;
	}
	// Calculate the end position of the read operation
	uint64_t end_position = start + static_cast<uint64_t>(length);
	if (length < 0 || end_position > size) {
		end_position = size; // Read until the end of the file
	}

	if (start >= size || start >= end_position) {
		return result; // If the start position is beyond the file size, return an empty block
	}

	// Read the file as directed by the start and end positions
	if (to_nearest_newline) {
		Polonius::Block next_newline = search(end_position, -1, "\n");
		if (next_newline.initialized()) {
			end_position = next_newline.start() + 1;
		}
	}
	uint64_t bytes_to_read = end_position - start;
	std::vector<char> buffer(bytes_to_read);

	// Move the file pointer to the start position
	fseeko64(file, static_cast<int64_t>(start), SEEK_SET);
	size_t bytes_read = fread(buffer.data(), 1, bytes_to_read, file);
	if (bytes_read == 0) {
		if (ferror(file)) {
			throw std::runtime_error("Error reading from file at position " + std::to_string(start));
		}
		return result; // EOF reached, return empty block
	}
	// If we read some data, set the contents of the result block
	result.add(start, std::string(buffer.data(), bytes_read));
	result.set_initialized();
	
	if (ferror(file)) {
		throw std::runtime_error("Error reading from file");
	}
	return result;
}

Polonius::Block Polonius::File::readLines(uint64_t start, int number_of_lines, bool stop_at_blocksize) const {
	Polonius::Block result;
	if (!file || start >= size) {
		return result;
	}

	uint64_t current_position = start;
	fseeko64(file, static_cast<int64_t>(current_position), SEEK_SET);
	std::string contents;
	std::string line;
	while (number_of_lines > 0 && (stop_at_blocksize ? result.get_contents().size() < Polonius::block_size : true)) {
		char c;
		if (fseeko64(file, static_cast<int64_t>(current_position), SEEK_SET) != 0 || fread(&c, 1, 1, file) != 1) {
			if (feof(file)) {
				break;
			}
			throw std::runtime_error("Error reading from file at position " + std::to_string(current_position));
		}
		line += c;
		switch (c) {
			case '\n':
				contents += line;
				line.clear();
				number_of_lines--;
				break;
			default:
				break;
		}
		current_position++;
	}
	if (!line.empty()) {
		contents += line;
	}
	result.add(start, contents);
	result.set_initialized();
	return result;
}

/**
 * @brief Reads lines from the file backwards starting from a specific position.
 * 
 * @param start The position to start reading from.
 * @param number_of_lines The number of lines to read.
 * @param stop_at_blocksize If true, stops reading when the block size is reached, regardless of line count.
 * 
 * @return A Polonius::Block containing the lines read from the file.
 * 		The end position of the block (its last byte) is the given 'start' position.
 */
Polonius::Block Polonius::File::readLines_backwards(uint64_t start, int number_of_lines, bool stop_at_blocksize) const {
	Polonius::Block result;
	if (!file || start >= size) {
		return result;
	}

	std::deque<std::string> lines;
	uint64_t current_position = start;
	fseeko64(file, static_cast<int64_t>(current_position), SEEK_SET);
	std::string contents;
	std::string line;
	bool hit_beginning_of_file = false;
	int lines_left = number_of_lines;
	while (lines_left >= 0 && (stop_at_blocksize ? result.get_contents().size() < Polonius::block_size : true)) {
		char c;
		if (fseeko64(file, static_cast<int64_t>(current_position), SEEK_SET) != 0 || fread(&c, 1, 1, file) != 1) {
			if (feof(file)) {
				break; // End of file reached
			}
			throw std::runtime_error("Error reading from file at position " + std::to_string(current_position));
		}
		switch (c) {
			case '\n':
				if (!line.empty()) {
					std::reverse(line.begin(), line.end());
					lines.push_front(line);
					line.clear();
					lines_left--;
				}
				line += c;
				break;
			default:
				line += c;
				break;
		}
		current_position--;
		if (hit_beginning_of_file) {
			break;
		}
		if (current_position == 0) {
			hit_beginning_of_file = true; // Read only one more character
		}
	}
	if (!line.empty() && line.back() != '\n') {
		std::reverse(line.begin(), line.end());
		lines.push_front(line); // Add the last line if it exists
	}
	for (const auto& l : lines) {
		contents += l;
	}

	// If we hit the beginning of the file early, we may not have read enough lines to satisfy the request
	// If this is the case, continue reading forward from the start position until we have enough lines
	if (lines.size() < static_cast<size_t>(number_of_lines)) {
		// Read forward from the start position until we have enough lines
		int difference = number_of_lines - static_cast<int>(lines.size()); // How many did we miss?
		contents += readLines(start + 1, difference, stop_at_blocksize).get_contents();
	}

	result.add(current_position + 1, contents);
	result.set_initialized();
	return result;
}

/**
 * @brief Searches for a specific query in the file.
 * 
 * This function searches for the specified search query in the file starting from the position defined by `Polonius::Reader::start_position`.
 * It reads the file in chunks of size `Polonius::block_size` and checks if the search query is present in each chunk.
 * If a match is found, it outputs the start and end positions of the match or the matched text, depending on the value of `Polonius::Reader::output_positions`.
 */
Polonius::Block Polonius::File::search(uint64_t start, int64_t length, const std::string& query) const {
	Polonius::Block result;
	// Calculate the end position of the read operation
	uint64_t end_position = start + static_cast<uint64_t>(length);
	if (length < 0 || end_position > size) {
		end_position = size; // Read until the end of the file
	}
	if (start >= size) {
		return result;
	}

	uint64_t bs = Polonius::block_size;

	uint64_t query_length = query.length();

	if (bs <= query_length) {
		bs = query_length + 1; // Ensure block size is at least the length of the search query
	}

	uint64_t shift_amount = bs - query_length;

	// Here's the procedure:
	// 1. Set start position to the start position
	// 1. Scan a chunk of block_size bytes from the start position for a match
	// 2. If a match is found, return it
	// 3. If a match is not found, set the start position to the next chunk minus the size of the search query
	// 4. Repeat until the end of the file is reached or a match is found
	// Setting the chunks in this way ensures that we don't miss any matches that might span across chunks.

	for (uint64_t pos = start; pos < end_position; pos += shift_amount) {
		uint64_t remaining = end_position - pos;
		if (bs > remaining || shift_amount > remaining) {
			bs = remaining;
			shift_amount = remaining;
		}

		std::string data = readFromFile(pos, static_cast<int64_t>(bs)).get_contents();

		// Check if the search query is in the data
		size_t search_result = data.find(query);
		if (search_result == std::string::npos) {
			continue; // No match found, continue to the next chunk
		}
		// Match found
		result.add(pos + search_result, data.substr(search_result, query_length));
		result.set_initialized();
		return result;
	}

	// If we reach here, no match was found in the entire file
	Polonius::exit_code = EXIT_FAILURE;
	return result;
}

/**
 * @brief Scans backwards in the file from the given start position for a specific query.
 */
Polonius::Block Polonius::File::search_backwards(uint64_t start, int64_t length, const std::string& query) const {
	Polonius::Block result;

	// Calculate the end position
	uint64_t end;

	if (length < 0 || start < static_cast<uint64_t>(length)) {
		end = 0;
	} else {
		end = start - static_cast<uint64_t>(length);
	}

	if (start > size) {
		start = size;
	}

	// Scan in block_size chunks backwards from the start position
	uint64_t bs = Polonius::block_size;
	if (bs <= query.length()) {
		bs = query.length() + 1; // Ensure block size is at least the length of the search query
	}
	for (uint64_t pos = start; pos >= end; pos -= bs) {
		uint64_t remaining = pos - end;
		if (remaining == 0) {
			break; // No more data to read
		}
		if (bs > remaining) {
			bs = remaining; // Adjust block size to the remaining bytes
		}

		std::string data = readFromFile(pos - bs, static_cast<int64_t>(bs)).get_contents();

		// Check if the search query is in the data
		size_t search_result = data.rfind(query);
		if (search_result == std::string::npos) {
			continue; // No match found, continue to the next chunk
		}
		// Match found
		result.add(pos - bs + search_result, data.substr(search_result, query.length()));
		result.set_initialized();
		return result;
	}
	// If we reach here, no match was found in the entire file
	Polonius::exit_code = EXIT_FAILURE;
	return result;
}

/**
 * @brief Performs a regex search on the file.
 * 
 * A regex search in Polonius should happen this way:
 * 1. Parse the regular expression into its component parts
 * 		e.g:
 * 		Expression: 'abc[a-z]+235'
 * 		Should be split into:
 * 		'a' 'b' 'c'
 * 		'[a-z]+'
 * 		'2' '3' '5'
 * 2. Recombine the components into multiple expressions:
 * 		a. 'abc[a-z]+235'
 * 		b. 'abc[a-z]+23'
 * 		c. 'abc[a-z]+2'
 * 		d. 'abc[a-z]+'
 * 		e. 'abc'
 * 		f. 'ab'
 * 		g. 'a'
 * 3. Scan the file in blocks of size `Polonius::block_size` for a match of the full expression (a)
 * 4. If no full match is found, check if the block ENDS with a PARTIAL MATCH? (any of b, c, d, e, f, g)
 * 5. If no, load the next block and repeat
 * 6. If yes, load a new block starting from the start position of the partial match
 * 7. Repeat until the end of the file is reached or a match is found
 */
Polonius::Block Polonius::File::regex_search(uint64_t start, int64_t length, const std::string& query) const {
	Polonius::Block result;
	// Calculate the end position of the read operation
	uint64_t end_position = start + static_cast<uint64_t>(length);
	if (length < 0 || end_position > size) {
		return result;
	}

	uint64_t bs = Polonius::block_size;
	if (bs <= query.length()) {
		bs = query.length() + 1; // Ensure block size is at least the length of the search query
	}
	std::vector<std::string> sub_expressions = create_sub_expressions(query);

	for (uint64_t pos = start; pos < end_position; pos += bs) {
		regex_scan:
		uint64_t remaining = end_position - pos;

		if (bs > remaining) {
			bs = remaining; // Adjust block size to the remaining bytes
		}

		std::string data = readFromFile(pos, static_cast<int64_t>(bs)).get_contents();
		boost::smatch regex_search_result;

		boost::regex expression(query);
		bool full_match_found = boost::regex_search(data, regex_search_result, expression);

		if (!full_match_found) {
			// Check for partial matches with sub-expressions
			for (const auto& subexpr : sub_expressions) {
				boost::smatch sub_expression_search_result;
				boost::regex sub_expression(subexpr + R"($)"); // 'R"($)"' signifies that the std::string must END with the match

				// Partial match found?
				bool partial_match_found = boost::regex_search(data, sub_expression_search_result, sub_expression);
				if (partial_match_found) {
					uint64_t partial_match_position = static_cast<uint64_t>(sub_expression_search_result.prefix().length());
					pos += partial_match_position; // Move to the next position after the partial match
					goto regex_scan; // Restart the loop with the new position
				}
			}
		} else {
			// Full match found
			result.add(pos + static_cast<uint64_t>(regex_search_result.prefix().length()), regex_search_result[0]);
			result.set_initialized();
			return result;
		}
	}

	// If we reach here, no match was found in the entire file
	Polonius::exit_code = EXIT_FAILURE;
	return result;
}
