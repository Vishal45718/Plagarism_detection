#include "similarity.h"
#include <unordered_set>
#include <cmath>
#include <algorithm>

namespace similarity {

    MatchResult calculate_similarity(const std::vector<fingerprinting::Fingerprint>& fp1,
                                     const std::vector<fingerprinting::Fingerprint>& fp2,
                                     const std::vector<int>& line_map1,
                                     const std::vector<int>& line_map2) {
        
        std::unordered_set<uint32_t> set1, set2;
        for (const auto& fp : fp1) set1.insert(fp.hash);
        for (const auto& fp : fp2) set2.insert(fp.hash);

        int intersection = 0;
        std::unordered_set<uint32_t> common_set;
        for (const auto& hash : set1) {
            if (set2.count(hash)) {
                intersection++;
                common_set.insert(hash);
            }
        }

        int union_size = set1.size() + set2.size() - intersection;
        
        MatchResult result;
        result.common_hashes = intersection;
        result.total_unique_hashes = union_size;
        
        std::unordered_set<int> lines1, lines2;
        for (const auto& fp : fp1) {
            if (common_set.count(fp.hash) && fp.start_index < line_map1.size()) {
                lines1.insert(line_map1[fp.start_index]);
            }
        }
        for (const auto& fp : fp2) {
            if (common_set.count(fp.hash) && fp.start_index < line_map2.size()) {
                lines2.insert(line_map2[fp.start_index]);
            }
        }
        result.matched_lines_file1.assign(lines1.begin(), lines1.end());
        result.matched_lines_file2.assign(lines2.begin(), lines2.end());
        std::sort(result.matched_lines_file1.begin(), result.matched_lines_file1.end());
        std::sort(result.matched_lines_file2.begin(), result.matched_lines_file2.end());

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

        result.cosine_similarity = 0.0;

        return result;
    }

    std::unordered_map<uint32_t, double> compute_idf(const std::vector<std::vector<fingerprinting::Fingerprint>>& corpus) {
        std::unordered_map<uint32_t, int> df;
        int N = corpus.size();
        
        for (const auto& doc : corpus) {
            std::unordered_set<uint32_t> unique_hashes;
            for (const auto& fp : doc) {
                unique_hashes.insert(fp.hash);
            }
            for (uint32_t hash : unique_hashes) {
                df[hash]++;
            }
        }

        std::unordered_map<uint32_t, double> idf;
        for (const auto& pair : df) {
            idf[pair.first] = std::log(static_cast<double>(N) / (1 + pair.second));
        }
        return idf;
    }

    double calculate_cosine_similarity(const std::vector<fingerprinting::Fingerprint>& fp1,
                                       const std::vector<fingerprinting::Fingerprint>& fp2,
                                       const std::unordered_map<uint32_t, double>& idf_map) {
        std::unordered_map<uint32_t, int> tf1, tf2;
        for (const auto& fp : fp1) tf1[fp.hash]++;
        for (const auto& fp : fp2) tf2[fp.hash]++;

        double dot_product = 0.0;
        double norm1 = 0.0;
        double norm2 = 0.0;

        auto tf_idf = [&](int count, double idf) { return count * idf; }; 

        for (const auto& pair : tf1) {
            double w = tf_idf(pair.second, idf_map.at(pair.first));
            norm1 += w * w;
        }
        for (const auto& pair : tf2) {
            double w = tf_idf(pair.second, idf_map.at(pair.first));
            norm2 += w * w;
            
            if (tf1.count(pair.first)) {
                double w1 = tf_idf(tf1[pair.first], idf_map.at(pair.first));
                dot_product += w1 * w;
            }
        }

        if (norm1 == 0.0 || norm2 == 0.0) return 0.0;
        return dot_product / (std::sqrt(norm1) * std::sqrt(norm2));
    }
}
