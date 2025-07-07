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

} // namespace Editor
} // namespace Polonius

#endif // SRC_EDIT_EDITOR_H_
