/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_READ_READER_H_
#define SRC_READ_READER_H_

#include <cstdint>
#include <string>
#include <iostream>

namespace Polonius {
namespace Reader {
extern bool output_positions; // Whether to output only positions of text

enum search_type : uint8_t {
	t_normal_search,
	t_regex_search
};

extern search_type search_mode; // The type of search to perform

extern std::string search_query; // The query to search for

extern uint64_t start_position; // The starting position for reading or searching
extern int64_t amount_to_read; // The amount of data to read, -1 means read until EOF

extern std::ostream& output_stream; // The output stream to write results to
} // namespace Reader
} // namespace Polonius

#endif // SRC_READ_READER_H_
