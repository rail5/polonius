/**
 * Copyright (C) 2023-2025 rail5
 */

#ifndef SRC_SHARED_PARSE_REGEX_H_
#define SRC_SHARED_PARSE_REGEX_H_

#include <string>
#include <vector>

std::vector<std::string> parse_regex(std::string expression);
std::vector<std::string> create_sub_expressions(std::string expression);

#endif // SRC_SHARED_PARSE_REGEX_H_
