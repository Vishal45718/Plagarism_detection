#include "file_utils.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <iostream>
#include <iomanip>

namespace fs = std::filesystem;

namespace utils {

    std::string read_file(const std::string& filepath) {
        std::ifstream file(filepath);
        if (!file.is_open()) {
            std::cerr << "Warning: Could not open file " << filepath << std::endl;
            return "";
        }
        std::ostringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    std::vector<std::string> get_files_in_directory(const std::string& directory) {
        std::vector<std::string> files;
        if (!fs::exists(directory) || !fs::is_directory(directory)) {
            std::cerr << "Warning: Directory not found or invalid: " << directory << std::endl;
            return files;
        }

        for (const auto& entry : fs::recursive_directory_iterator(directory)) {
            if (fs::is_regular_file(entry)) {
                files.push_back(entry.path().string());
            }
        }
        return files;
    }

    std::string escape_json(const std::string& s) {
        std::ostringstream o;
        for (auto c : s) {
            switch (c) {
                case '"': o << "\\\""; break;
                case '\\': o << "\\\\"; break;
                case '\b': o << "\\b"; break;
                case '\f': o << "\\f"; break;
                case '\n': o << "\\n"; break;
                case '\r': o << "\\r"; break;
                case '\t': o << "\\t"; break;
                default:
                    if ('\x00' <= c && c <= '\x1f') {
                        o << "\\u" << std::hex << std::setw(4) << std::setfill('0') << static_cast<int>(c);
                    } else {
                        o << c;
                    }
            }
        }
        return o.str();
    }
}
