#include "similarity.h"
#include <unordered_set>

namespace similarity {

    MatchResult calculate_similarity(const std::vector<fingerprinting::Fingerprint>& fp1,
                                     const std::vector<fingerprinting::Fingerprint>& fp2) {
        
        std::unordered_set<uint32_t> set1, set2;
        for (const auto& fp : fp1) set1.insert(fp.hash);
        for (const auto& fp : fp2) set2.insert(fp.hash);

        int intersection = 0;
        for (const auto& hash : set1) {
            if (set2.count(hash)) {
                intersection++;
            }
        }

        int union_size = set1.size() + set2.size() - intersection;
        
        MatchResult result;
        result.common_hashes = intersection;
        result.total_unique_hashes = union_size;
        
        if (union_size == 0) {
            result.jaccard_similarity = 0.0;
        } else {
            result.jaccard_similarity = static_cast<double>(intersection) / union_size;
        }

        if (set1.empty()) {
            result.containment = (set2.empty() ? 1.0 : 0.0);
        } else {
            result.containment = static_cast<double>(intersection) / set1.size();
        }

        return result;
    }
}
