#include "minhash.h"
#include <limits>
#include <algorithm>

namespace fingerprinting {

    // A simple hash function for MinHash: (a*x + b) % c
    // For production, we'd use better random seeds, but static is fine here.
    inline uint32_t hash_func(uint32_t x, uint32_t a, uint32_t b) {
        // A large prime
        const uint32_t c = 4294967291;
        return (static_cast<uint64_t>(a) * x + b) % c;
    }

    MinHashSignature compute_minhash_signature(const std::vector<Fingerprint>& fingerprints, int num_hashes) {
        MinHashSignature sig;
        sig.signature.assign(num_hashes, std::numeric_limits<uint32_t>::max());

        // Seed constants for num_hashes hash functions
        std::vector<uint32_t> a(num_hashes);
        std::vector<uint32_t> b(num_hashes);
        for (int i = 0; i < num_hashes; ++i) {
            a[i] = (i * 1337 + 1) % 4294967291;
            b[i] = (i * 8191 + 7) % 4294967291;
        }

        for (const auto& fp : fingerprints) {
            for (int i = 0; i < num_hashes; ++i) {
                uint32_t h = hash_func(fp.hash, a[i], b[i]);
                if (h < sig.signature[i]) {
                    sig.signature[i] = h;
                }
            }
        }

        return sig;
    }

    double estimate_jaccard(const MinHashSignature& sig1, const MinHashSignature& sig2) {
        if (sig1.signature.empty() || sig1.signature.size() != sig2.signature.size()) return 0.0;
        int matches = 0;
        for (size_t i = 0; i < sig1.signature.size(); ++i) {
            if (sig1.signature[i] == sig2.signature[i]) {
                matches++;
            }
        }
        return static_cast<double>(matches) / sig1.signature.size();
    }
}
