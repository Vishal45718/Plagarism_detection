#ifndef MINHASH_H
#define MINHASH_H

#include "winnowing.h"
#include <vector>
#include <cstdint>

namespace fingerprinting {

    struct MinHashSignature {
        std::vector<uint32_t> signature;
    };

    // Computes a MinHash signature given a list of fingerprints.
    // num_hashes controls the size of the signature.
    MinHashSignature compute_minhash_signature(const std::vector<Fingerprint>& fingerprints, int num_hashes = 100);

    // Estimate Jaccard similarity using MinHash signatures.
    double estimate_jaccard(const MinHashSignature& sig1, const MinHashSignature& sig2);
}

#endif
