#include "preprocessor.h"
#include <cctype>

namespace preprocessing {

    NormalizedDocument preprocess(const std::string& raw_text) {
        NormalizedDocument doc;
        doc.content.reserve(raw_text.size());
        doc.index_map.reserve(raw_text.size());

        std::size_t i = 0;
        std::size_t n = raw_text.length();

        while (i < n) {
            // Check for block comment /* */
            if (i + 1 < n && raw_text[i] == '/' && raw_text[i+1] == '*') {
                i += 2;
                while (i + 1 < n && !(raw_text[i] == '*' && raw_text[i+1] == '/')) {
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

            // Otherwise, it's a valid character. Normalize to lowercase.
            doc.content.push_back(static_cast<char>(std::tolower(static_cast<unsigned char>(raw_text[i]))));
            doc.index_map.push_back(i);
            i++;
        }

        return doc;
    }

}
