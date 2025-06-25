// preprocessing.cpp
#include "preprocessing.h"
#include "utils.h"
#include <sstream>
#include <fstream>
#include <unordered_set>
#include <algorithm>

std::vector<std::string> preprocess(const std::string &text) {
    std::vector<std::string> tokens;
    std::unordered_set<std::string> stopwords = load_stopwords("stopwords.txt");

    std::stringstream ss(text);
    std::string word;
    while (ss >> word) {
        std::transform(word.begin(), word.end(), word.begin(), ::tolower);
        word.erase(remove_if(word.begin(), word.end(), ::ispunct), word.end());
        if (!stopwords.count(word) && !word.empty()) {
            tokens.push_back(word);
        }
    }
    return tokens;
}
