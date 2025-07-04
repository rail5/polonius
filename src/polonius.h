/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_POLONIUS_H_
#define SRC_POLONIUS_H_

#include <cstdint>
#include <deque>
#include <filesystem>

#include "block.h"
#include "edit/editor.h"
#include "read/reader.h"

namespace Polonius {

// Forward declarations
namespace Editor {
class Expression;
} // namespace Editor

namespace Reader {
enum search_type : uint8_t;
} // namespace Reader

extern uint8_t exit_code; // Exit code for the program
extern uint64_t block_size;
extern bool editor_mode;
extern bool reader_mode;
extern bool special_chars; // Whether to process escape sequences in strings

} // namespace Polonius

#endif // SRC_POLONIUS_H_
