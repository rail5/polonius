#include <iostream>
#include <fstream>
#include <cstdint>
#include <vector>
#include <iomanip>
#include <getopt.h>

/* Needed for file_exists() */
#include <sys/stat.h>

/* Needed for class_file */
#include <filesystem>

/* Needed for to_lower */
#include <algorithm>

#include "../../shared_functions/file_exists.cpp"
#include "../../shared_functions/isolate_path_from_filename.cpp"
#include "../../shared_functions/to_lower.cpp"
#include "../../shared_functions/explode.cpp"
#include "../../shared_functions/is_number.cpp"

#include "namespace_editor.h"
#include "class_instruction.cpp"
#include "class_file.cpp"
