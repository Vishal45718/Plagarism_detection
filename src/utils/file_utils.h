#ifndef FILE_UTILS_H
#define FILE_UTILS_H

#include <string>
#include <vector>

namespace utils {
    // Reads an entire file into a string
    std::string read_file(const std::string& filepath);

    // Recursively gets all files in a directory that match
    std::vector<std::string> get_files_in_directory(const std::string& directory);

    // Escape a string to be JSON safe
    std::string escape_json(const std::string& s);
}

#endif // FILE_UTILS_H
