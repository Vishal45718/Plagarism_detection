#ifndef SIMILARITY_H
#define SIMILARITY_H

#include "../fingerprinting/winnowing.h"
#include <unordered_map>
#include <vector>

namespace similarity {

struct MatchResult {
  double jaccard_similarity;
  double cosine_similarity;
  double containment;
  int common_hashes;
  int total_unique_hashes;
  std::vector<int> matched_lines_file1;
  std::vector<int> matched_lines_file2;
};

// Calculates Jaccard and containment
MatchResult calculate_similarity(const std::vector<fingerprinting::Fingerprint> &fp1,
                                 const std::vector<fingerprinting::Fingerprint> &fp2,
                                 const std::vector<int>& line_map1,
                                 const std::vector<int>& line_map2);

// Calculates Cosine similarity using TF-IDF
double calculate_cosine_similarity(
    const std::vector<fingerprinting::Fingerprint> &fp1,
    const std::vector<fingerprinting::Fingerprint> &fp2,
    const std::unordered_map<uint32_t, double> &idf_map);

// Computes IDF map
std::unordered_map<uint32_t, double> compute_idf(
    const std::vector<std::vector<fingerprinting::Fingerprint>> &corpus);
} // namespace similarity

#endif
