#ifndef WINNOWING_H
#define WINNOWING_H

#include <vector>
#include <string>
#include <cstdint>
#include <cstddef>

namespace fingerprinting {

    struct Fingerprint {
        uint32_t hash;
        std::size_t start_index; // Index in the normalized content
    };

    // Calculate rolling hashes (k-grams), then pick minimum in a sliding window (winnowing).
    // k: size of k-gram (e.g. 15 characters, assuming normalized text)
    // t: guarantee threshold (if a match is >= t characters, it's guaranteed to be detected)
    std::vector<Fingerprint> compute_fingerprints(const std::string& text, int k = 15, int t = 20);

}

#endif
