#include "preprocessor.h"
#include <cctype>
#include <unordered_set>
#include <algorithm>

namespace preprocessing {

    struct LanguageConfig {
        std::unordered_set<std::string> keywords;
        
        LanguageConfig() {
            // Centralized configuration for keywords across supported languages
            const char* words[] = {
                // C++
                "if", "else", "while", "for", "do", "break", "continue", "return",
                "switch", "case", "default", "try", "catch", "throw", "class", "struct",
                "int", "float", "double", "char", "void", "bool", "public", "private", "protected",
                "std", "vector", "string", "cout", "cin", "endl", "include", "define",
                // Python
                "def", "import", "from", "as", "pass", "and", "or", "not", "is", "in",
                "True", "False", "None", "elif", "print", "except", "finally", "raise"
            };
            for (const char* w : words) keywords.insert(w);
        }
        
        bool is_keyword(const std::string& word) const {
            return keywords.find(word) != keywords.end();
        }
    };
    
    const LanguageConfig LANG_CONFIG;

    NormalizedDocument preprocess(const std::string& raw_text) {
        NormalizedDocument doc;
        doc.content.reserve(raw_text.size());
        doc.index_map.reserve(raw_text.size());
        doc.line_map.reserve(raw_text.size());

        std::size_t i = 0;
        std::size_t n = raw_text.length();
        int current_line = 1;

        while (i < n) {
            if (raw_text[i] == '\n') {
                current_line++;
                i++;
                continue;
            }

            // String literals
            if (raw_text[i] == '"') {
                i++;
                while (i < n && raw_text[i] != '"') {
                    if (raw_text[i] == '\\' && i + 1 < n) {
                        if (raw_text[i+1] == '\n') current_line++;
                        i += 2;
                    } else {
                        if (raw_text[i] == '\n') current_line++;
                        i++;
                    }
                }
                if (i < n) i++; // skip closing quote
                continue;
            }

            // Char literals
            if (raw_text[i] == '\'') {
                i++;
                while (i < n && raw_text[i] != '\'') {
                    if (raw_text[i] == '\\' && i + 1 < n) {
                        if (raw_text[i+1] == '\n') current_line++;
                        i += 2;
                    } else {
                        if (raw_text[i] == '\n') current_line++;
                        i++;
                    }
                }
                if (i < n) i++;
                continue;
            }

            // Check for block comment /* */
            if (i + 1 < n && raw_text[i] == '/' && raw_text[i+1] == '*') {
                i += 2;
                while (i + 1 < n && !(raw_text[i] == '*' && raw_text[i+1] == '/')) {
                    if (raw_text[i] == '\n') current_line++;
                    i++;
                }
                i += 2; // skip */
                continue;
            }

            // Check for line comment //
            if (i + 1 < n && raw_text[i] == '/' && raw_text[i+1] == '/') {
                i += 2;
                while (i < n && raw_text[i] != '\n') {
                    i++;
                }
                continue;
            }

            // Check for python line comment #
            if (raw_text[i] == '#') {
                i++;
                while (i < n && raw_text[i] != '\n') {
                    i++;
                }
                continue;
            }

            // Skip whitespaces
            if (std::isspace(static_cast<unsigned char>(raw_text[i]))) {
                i++;
                continue;
            }

            // Tokenize identifiers (alphanumeric and underscore)
            if (std::isalpha(static_cast<unsigned char>(raw_text[i])) || raw_text[i] == '_') {
                std::string word = "";
                std::size_t start_i = i;
                while (i < n && (std::isalnum(static_cast<unsigned char>(raw_text[i])) || raw_text[i] == '_')) {
                    word += raw_text[i];
                    i++;
                }
                
                std::string token_to_push = word;
                if (!LANG_CONFIG.is_keyword(word)) {
                    token_to_push = "V"; // Generic variable token
                } else {
                    // Convert keyword to lowercase for normalization
                    std::transform(token_to_push.begin(), token_to_push.end(), token_to_push.begin(),
                        [](unsigned char c){ return std::tolower(c); });
                }

                for (char c : token_to_push) {
                    doc.content.push_back(c);
                    doc.index_map.push_back(start_i); // map to the start of the word
                    doc.line_map.push_back(current_line);
                }
                continue;
            }

            // Otherwise, it's a valid character. Normalize to lowercase.
            doc.content.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(raw_text[i]))));
            doc.index_map.push_back(i);
            doc.line_map.push_back(current_line);
            i++;
        }

        return doc;
    }
}
