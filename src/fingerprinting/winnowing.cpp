#include "winnowing.h"
#include <deque>

namespace fingerprinting {

    struct HashInfo {
        uint32_t hash;
        std::size_t index;
    };

    std::vector<HashInfo> compute_kgram_hashes(const std::string& text, int k) {
        std::vector<HashInfo> hashes;
        if (text.length() < static_cast<std::size_t>(k)) return hashes;

        const uint32_t base = 31;
        uint32_t current_hash = 0;
        uint32_t highest_power = 1;

        for (int i = 0; i < k - 1; ++i) {
            highest_power *= base;
        }

        // Initialize the first k-gram
        for (int i = 0; i < k; ++i) {
            current_hash = current_hash * base + static_cast<unsigned char>(text[i]);
        }
        hashes.push_back({current_hash, 0});

        // Rolling hash for the rest
        for (std::size_t i = k; i < text.length(); ++i) {
            current_hash = (current_hash - static_cast<unsigned char>(text[i - k]) * highest_power) * base + static_cast<unsigned char>(text[i]);
            hashes.push_back({current_hash, i - k + 1});
        }

        return hashes;
    }

    std::vector<Fingerprint> compute_fingerprints(const std::string& text, int k, int t) {
        std::vector<Fingerprint> fingerprints;
        if (text.length() < static_cast<std::size_t>(k)) return fingerprints;

        int w = t - k + 1;
        if (w < 1) w = 1;

        std::vector<HashInfo> hashes = compute_kgram_hashes(text, k);
        std::deque<HashInfo> window;

        for (std::size_t i = 0; i < hashes.size(); ++i) {
            // Remove elements outside the current window
            while (!window.empty() && i >= static_cast<std::size_t>(w) && window.front().index <= i - static_cast<std::size_t>(w)) {
                window.pop_front();
            }

            // Pop elements greater than or equal to current hash to keep rightmost minimum at the front
            // Note: we use >= so that the rightmost minimum is preferred.
            while (!window.empty() && window.back().hash >= hashes[i].hash) {
                window.pop_back();
            }

            window.push_back(hashes[i]);

            // When a full window is formed, record the finger print
            if (i >= static_cast<std::size_t>(w) - 1) {
                // To avoid duplicate fingerprints, check against the last recorded one
                if (fingerprints.empty() || fingerprints.back().start_index != window.front().index) {
                    fingerprints.push_back({window.front().hash, window.front().index});
                }
            }
        }

        return fingerprints;
    }
}
