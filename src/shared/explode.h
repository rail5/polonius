/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SHARED_EXPLODE_H_
#define SHARED_EXPLODE_H_

#include <string>
#include <vector>

std::vector<std::string> explode(const std::string& input, char delimiter, bool can_escape = false, int maximum_number_of_elements = 0);

#endif // SHARED_EXPLODE_H_
