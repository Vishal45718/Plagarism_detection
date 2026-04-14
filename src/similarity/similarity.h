#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "../fingerprinting/winnowing.h"
#include <vector>

namespace similarity {

    struct MatchResult {
        double jaccard_similarity;
        double containment;       // percentage of doc1 hashes found in doc2
        int common_hashes;        // size of intersection of hashes
        int total_unique_hashes;  // size of union of hashes
    };

    MatchResult calculate_similarity(const std::vector<fingerprinting::Fingerprint>& fp1,
                                     const std::vector<fingerprinting::Fingerprint>& fp2);
}

#endif
