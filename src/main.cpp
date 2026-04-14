#include <iostream>
#include <vector>
#include <string>
#include <iomanip>
#include <filesystem>
#include <algorithm>
#include "utils/file_utils.h"
#include "preprocessing/preprocessor.h"
#include "fingerprinting/winnowing.h"
#include "similarity/similarity.h"

using namespace std;
namespace fs = std::filesystem;

void print_usage() {
    cout << "Usage: plagiarism_detector [options]\n"
         << "Options:\n"
         << "  --dir <directory>       Directory containing files to compare\n"
         << "  --threshold <0.0-1.0>   Minimum Jaccard similarity threshold to report (default: 0.1)\n"
         << "  --kgram <int>           Size of k-grams (default: 15)\n"
         << "  --window <int>          Winnowing window size guarantee threshold (default: 20)\n";
}

int main(int argc, char** argv) {
    string target_dir = "";
    double threshold = 0.1;
    int kgram = 15;
    int window_thresh = 20;

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];
        if (arg == "--dir" && i + 1 < argc) {
            target_dir = argv[++i];
        } else if (arg == "--threshold" && i + 1 < argc) {
            threshold = stod(argv[++i]);
        } else if (arg == "--kgram" && i + 1 < argc) {
            kgram = stoi(argv[++i]);
        } else if (arg == "--window" && i + 1 < argc) {
            window_thresh = stoi(argv[++i]);
        } else {
            print_usage();
            return 1;
        }
    }

    if (target_dir.empty()) {
        cerr << "Error: --dir argument is required.\n";
        print_usage();
        return 1;
    }

    auto files = utils::get_files_in_directory(target_dir);
    if (files.empty()) {
        cerr << "No files found in directory: " << target_dir << endl;
        return 0;
    }

    // Precompute fingerprints for all files
    struct FileData {
        string path;
        vector<fingerprinting::Fingerprint> fp;
    };

    vector<FileData> dataset;
    for (const auto& f : files) {
        string raw_content = utils::read_file(f);
        auto norm_doc = preprocessing::preprocess(raw_content);
        auto fp = fingerprinting::compute_fingerprints(norm_doc.content, kgram, window_thresh);
        dataset.push_back({f, fp});
    }

    // Pairwise comparison
    cout << "{\n  \"threshold_used\": " << threshold << ",\n";
    cout << "  \"kgram\": " << kgram << ",\n";
    cout << "  \"window_thresh\": " << window_thresh << ",\n";
    cout << "  \"matches\": [\n";

    bool first = true;
    for (size_t i = 0; i < dataset.size(); ++i) {
        for (size_t j = i + 1; j < dataset.size(); ++j) {
            auto sim = similarity::calculate_similarity(dataset[i].fp, dataset[j].fp);
            if (sim.jaccard_similarity >= threshold) {
                if (!first) cout << ",\n";
                first = false;
                
                cout << "    {\n";
                cout << "      \"file1\": \"" << utils::escape_json(dataset[i].path) << "\",\n";
                cout << "      \"file2\": \"" << utils::escape_json(dataset[j].path) << "\",\n";
                cout << "      \"jaccard_similarity\": " << fixed << setprecision(4) << sim.jaccard_similarity << ",\n";
                // File 1 contained in File 2 containment
                cout << "      \"containment_file1_in_file2\": " << fixed << setprecision(4) << sim.containment << ",\n";
                // File 2 contained in File 1 containment
                auto sim21 = similarity::calculate_similarity(dataset[j].fp, dataset[i].fp);
                cout << "      \"containment_file2_in_file1\": " << fixed << setprecision(4) << sim21.containment << ",\n";
                cout << "      \"common_hashes\": " << sim.common_hashes << "\n";
                cout << "    }";
            }
        }
    }
    cout << "\n  ]\n}\n";

    return 0;
}
