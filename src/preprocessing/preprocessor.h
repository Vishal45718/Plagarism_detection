#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <vector>
#include <cstddef>

namespace preprocessing {
    
    struct NormalizedDocument {
        std::string content;
        std::vector<std::size_t> index_map; // map normalized content char index -> original text char index
        std::vector<int> line_map;          // map normalized content char index -> original text line number
    };

    // Preprocesses the raw text:
    // - Strips comments (//, /* */, #)
    // - Strips whitespaces
    // - Normalizes identifiers to a generic token 'V'
    // - Converts to lowercase
    NormalizedDocument preprocess(const std::string& raw_text);
}

#endif
