#pragma once
#include <string>
#include <unordered_set>

std::string read_file(const std::string &filename);
std::unordered_set<std::string> load_stopwords(const std::string &filename);
