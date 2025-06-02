/***
 * Copyright (C) 2024 rail5
*/

#include "is_number.h"
#include <algorithm>

bool is_number(const std::string &s) {
    return !s.empty() && std::find_if(s.begin(), 
        s.end(), [](unsigned char c) { return !std::isdigit(c); }) == s.end();
}
