#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <chrono>
#include <unordered_map>
#include "cli/cli.h"
#include "utils/file_utils.h"
#include "preprocessing/preprocessor.h"
#include "fingerprinting/winnowing.h"
#include "fingerprinting/minhash.h"
#include "similarity/similarity.h"
#include "reporting/report.h"

using namespace std;

struct Document {
    string path;
    preprocessing::NormalizedDocument norm_doc;
    vector<fingerprinting::Fingerprint> fp;
    fingerprinting::MinHashSignature minhash_sig;
};

bool lsh_candidate(const fingerprinting::MinHashSignature& sig1, const fingerprinting::MinHashSignature& sig2, int bands = 20, int rows = 5) {
    if (sig1.signature.size() != static_cast<size_t>(bands * rows)) return true; // fallback if wrong size
    if (sig2.signature.size() != static_cast<size_t>(bands * rows)) return true;

    for (int b = 0; b < bands; ++b) {
        bool match = true;
        for (int r = 0; r < rows; ++r) {
            int idx = b * rows + r;
            if (sig1.signature[idx] != sig2.signature[idx]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

int main(int argc, char** argv) {
    cli::Config config = cli::parse_args(argc, argv);
    if (!config.valid) {
        if (!config.error_msg.empty()) {
            cerr << "Error: " << config.error_msg << "\n";
            cli::print_help();
        }
        return 1;
    }

    auto files = utils::get_files_in_directory(config.target_dir, config.recursive);
    if (files.empty()) {
        cerr << "Warning: No files found in directory: " << config.target_dir << "\n";
        return 0;
    }

    if (config.verbose) {
        cerr << "[INFO] Found " << files.size() << " files.\n";
        cerr << "[INFO] Preprocessing and computing fingerprints...\n";
    }

    vector<Document> dataset;
    dataset.reserve(files.size());

    const int minhash_size = 100; // 20 bands * 5 rows

    for (const auto& f : files) {
        string raw_content = utils::read_file(f);
        auto norm_doc = preprocessing::preprocess(raw_content);
        auto fp = fingerprinting::compute_fingerprints(norm_doc.content, config.kgram, config.window_thresh);
        auto minhash_sig = fingerprinting::compute_minhash_signature(fp, minhash_size);
        dataset.push_back({f, norm_doc, fp, minhash_sig});
    }

    unordered_map<uint32_t, double> idf_map;
    if (config.metric == cli::Metric::COSINE || config.metric == cli::Metric::BOTH) {
        if (config.verbose) cerr << "[INFO] Computing IDF map for Cosine Similarity...\n";
        vector<vector<fingerprinting::Fingerprint>> corpus;
        for (const auto& doc : dataset) corpus.push_back(doc.fp);
        idf_map = similarity::compute_idf(corpus);
    }

    if (config.verbose) cerr << "[INFO] Running pairwise comparisons using LSH...\n";

    vector<reporting::PairMatch> matches;
    int total_comparisons = 0;
    int actual_detailed_comparisons = 0;

    for (size_t i = 0; i < dataset.size(); ++i) {
        for (size_t j = i + 1; j < dataset.size(); ++j) {
            total_comparisons++;
            
            // LSH to avoid full O(n^2) detailed comparisons
            if (!lsh_candidate(dataset[i].minhash_sig, dataset[j].minhash_sig, 20, 5)) {
                // Even if not a candidate, we might want to show it in verbose mode if asked,
                // but the prompt says LSH is to AVOID O(n^2) comparisons. So we skip.
                continue;
            }
            
            actual_detailed_comparisons++;

            similarity::MatchResult metrics = similarity::calculate_similarity(dataset[i].fp, dataset[j].fp, dataset[i].norm_doc.line_map, dataset[j].norm_doc.line_map);
            
            if (config.metric == cli::Metric::COSINE || config.metric == cli::Metric::BOTH) {
                metrics.cosine_similarity = similarity::calculate_cosine_similarity(dataset[i].fp, dataset[j].fp, idf_map);
            }

            double primary_score = (config.metric == cli::Metric::COSINE) ? metrics.cosine_similarity : metrics.jaccard_similarity;
            
            if (config.verbose || primary_score >= config.threshold || config.top_n > 0) {
                matches.push_back({dataset[i].path, dataset[j].path, metrics, primary_score});
            }
        }
    }

    // Sort matches
    std::sort(matches.begin(), matches.end(), [](const reporting::PairMatch& a, const reporting::PairMatch& b) {
        return a.primary_score > b.primary_score;
    });

    // Apply threshold if not verbose and not Top-N mode
    if (!config.verbose && config.top_n <= 0) {
        auto it = std::remove_if(matches.begin(), matches.end(), [&](const reporting::PairMatch& m) {
            return m.primary_score < config.threshold;
        });
        matches.erase(it, matches.end());
    }

    // Apply Top-N
    if (config.top_n > 0 && matches.size() > static_cast<size_t>(config.top_n)) {
        matches.resize(config.top_n);
    }

    if (config.verbose) {
        cerr << "[INFO] Total possible pairs: " << total_comparisons << "\n";
        cerr << "[INFO] Pairs after LSH filtering: " << actual_detailed_comparisons << "\n";
        cerr << "[INFO] Final reported matches: " << matches.size() << "\n";
    }

    if (matches.empty()) {
        cerr << "Warning: No matches found.\n";
        cerr << "Tip: Try adjusting parameters with --mode sensitive or lowering the --threshold.\n";
    }

    reporting::generate_json_report(matches, config, dataset.size(), actual_detailed_comparisons);
    reporting::generate_html_report(matches, config, dataset.size(), actual_detailed_comparisons);

    return 0;
}
