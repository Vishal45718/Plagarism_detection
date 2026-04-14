#ifndef PREPROCESSOR_H
#define PREPROCESSOR_H

#include <string>
#include <vector>
#include <cstddef>

namespace preprocessing {
    
    struct NormalizedDocument {
        std::string content;
        std::vector<std::size_t> index_map; // map normalized content char index -> original text char index
    };

    // Preprocesses the raw text:
    // - Strips comments (//, /* */, #)
    // - Strips whitespaces
    // - Converts to lowercase
    NormalizedDocument preprocess(const std::string& raw_text);
}

#endif
