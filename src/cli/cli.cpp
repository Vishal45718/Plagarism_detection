#include "cli.h"
#include <iostream>
#include <vector>
#include <algorithm>

namespace cli {

    void print_help() {
        std::cout << "Usage: plagiarism_detector [options]\n"
                  << "Options:\n"
                  << "  --dir <directory>       Directory containing files to compare (required)\n"
                  << "  --threshold <float>     Minimum similarity threshold to report (default: 0.1)\n"
                  << "  --kgram <int>           Size of k-grams (default: 15)\n"
                  << "  --window <int>          Winnowing window size guarantee threshold (default: 20)\n"
                  << "  --verbose               Show all pairwise similarities and stats\n"
                  << "  --top <N>               Return top N most similar pairs regardless of threshold\n"
                  << "  --mode <mode>           Parameter modes: sensitive, balanced, strict\n"
                  << "  --metric <metric>       Similarity metric: jaccard, cosine, both\n"
                  << "  --recursive             Scan subdirectories recursively\n"
                  << "  --output <file.html>    Generate HTML report\n"
                  << "\nExamples:\n"
                  << "  plagiarism_detector --dir ./code --mode strict --metric both --output report.html\n"
                  << "  plagiarism_detector --dir ./src --recursive --top 10 --verbose\n";
    }

    Config parse_args(int argc, char** argv) {
        Config config;
        bool mode_set = false;
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            if (arg == "--dir" && i + 1 < argc) {
                config.target_dir = argv[++i];
            } else if (arg == "--threshold" && i + 1 < argc) {
                config.threshold = std::stod(argv[++i]);
            } else if (arg == "--kgram" && i + 1 < argc) {
                config.kgram = std::stoi(argv[++i]);
            } else if (arg == "--window" && i + 1 < argc) {
                config.window_thresh = std::stoi(argv[++i]);
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--top" && i + 1 < argc) {
                config.top_n = std::stoi(argv[++i]);
            } else if (arg == "--recursive") {
                config.recursive = true;
            } else if (arg == "--output" && i + 1 < argc) {
                config.output_html = argv[++i];
            } else if (arg == "--mode" && i + 1 < argc) {
                std::string mode_str = argv[++i];
                if (mode_str == "sensitive") config.mode = Mode::SENSITIVE;
                else if (mode_str == "balanced") config.mode = Mode::BALANCED;
                else if (mode_str == "strict") config.mode = Mode::STRICT;
                else {
                    config.valid = false;
                    config.error_msg = "Invalid mode: " + mode_str;
                    return config;
                }
                mode_set = true;
            } else if (arg == "--metric" && i + 1 < argc) {
                std::string metric_str = argv[++i];
                if (metric_str == "jaccard") config.metric = Metric::JACCARD;
                else if (metric_str == "cosine") config.metric = Metric::COSINE;
                else if (metric_str == "both") config.metric = Metric::BOTH;
                else {
                    config.valid = false;
                    config.error_msg = "Invalid metric: " + metric_str;
                    return config;
                }
            } else if (arg == "--help" || arg == "-h") {
                print_help();
                config.valid = false;
                return config;
            } else {
                config.valid = false;
                config.error_msg = "Unknown argument: " + arg;
                return config;
            }
        }

        if (config.target_dir.empty() && config.valid && config.error_msg.empty()) {
            config.valid = false;
            config.error_msg = "--dir is required.";
        }

        if (mode_set) {
            if (config.mode == Mode::SENSITIVE) {
                config.kgram = 10;
                config.window_thresh = 15;
                config.threshold = 0.05;
            } else if (config.mode == Mode::STRICT) {
                config.kgram = 20;
                config.window_thresh = 30;
                config.threshold = 0.3;
            }
            // balanced keeps defaults or overridden values from args if we want, but simple override here.
        }

        return config;
    }
}
