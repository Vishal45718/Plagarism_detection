#include "cli.h"
#include <iostream>
#include <vector>
#include <algorithm>
#include <cerrno>
#include <cmath>
#include <cstdlib>
#include <limits>

namespace cli {
    namespace {
        bool parse_double(const std::string& value, double& parsed) {
            char* end = nullptr;
            errno = 0;
            parsed = std::strtod(value.c_str(), &end);
            return errno != ERANGE && end != value.c_str() && *end == '\0' && std::isfinite(parsed);
        }

        bool parse_int(const std::string& value, int& parsed) {
            char* end = nullptr;
            errno = 0;
            long result = std::strtol(value.c_str(), &end, 10);
            if (errno == ERANGE || end == value.c_str() || *end != '\0') {
                return false;
            }
            if (result < std::numeric_limits<int>::min() || result > std::numeric_limits<int>::max()) {
                return false;
            }
            parsed = static_cast<int>(result);
            return true;
        }

        void set_error(Config& config, const std::string& message) {
            config.valid = false;
            config.error_msg = message;
        }
    }

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
                  << "  --bands <int>           LSH bands (default: 20)\n"
                  << "  --rows <int>            LSH rows (default: 5)\n"
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
                std::string value = argv[++i];
                if (!parse_double(value, config.threshold)) {
                    set_error(config, "Invalid threshold value: expected a number between 0 and 1");
                    return config;
                }
            } else if (arg == "--kgram" && i + 1 < argc) {
                std::string value = argv[++i];
                if (!parse_int(value, config.kgram)) {
                    set_error(config, "Invalid kgram value: expected a positive integer");
                    return config;
                }
            } else if (arg == "--window" && i + 1 < argc) {
                std::string value = argv[++i];
                if (!parse_int(value, config.window_thresh)) {
                    set_error(config, "Invalid window value: expected an integer greater than kgram");
                    return config;
                }
            } else if (arg == "--verbose") {
                config.verbose = true;
            } else if (arg == "--top" && i + 1 < argc) {
                std::string value = argv[++i];
                if (!parse_int(value, config.top_n)) {
                    set_error(config, "Invalid top value: expected a positive integer");
                    return config;
                }
            } else if (arg == "--recursive") {
                config.recursive = true;
            } else if (arg == "--output" && i + 1 < argc) {
                config.output_html = argv[++i];
            } else if (arg == "--bands" && i + 1 < argc) {
                std::string value = argv[++i];
                if (!parse_int(value, config.lsh_bands)) {
                    set_error(config, "Invalid bands value: expected a positive integer");
                    return config;
                }
            } else if (arg == "--rows" && i + 1 < argc) {
                std::string value = argv[++i];
                if (!parse_int(value, config.lsh_rows)) {
                    set_error(config, "Invalid rows value: expected a positive integer");
                    return config;
                }
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
                config.lsh_bands = 40;
                config.lsh_rows = 3;
            } else if (config.mode == Mode::STRICT) {
                config.kgram = 20;
                config.window_thresh = 30;
                config.threshold = 0.3;
                config.lsh_bands = 15;
                config.lsh_rows = 6;
            }
            // balanced keeps defaults or overridden values from args if we want, but simple override here.
        }

        if (config.threshold < 0.0 || config.threshold > 1.0) {
            set_error(config, "Invalid threshold value: expected a number between 0 and 1");
        } else if (config.kgram <= 0) {
            set_error(config, "Invalid kgram value: expected a positive integer");
        } else if (config.window_thresh <= config.kgram) {
            set_error(config, "Invalid window value: expected an integer greater than kgram");
        } else if (config.top_n == 0 || config.top_n < -1) {
            set_error(config, "Invalid top value: expected a positive integer");
        } else if (config.lsh_bands <= 0 || config.lsh_rows <= 0) {
            set_error(config, "LSH bands and rows must be positive integers.");
        }

        if (config.valid && config.error_msg.empty()) {
            if (config.lsh_bands < 10 || config.lsh_rows > 10) {
                std::cerr << "[WARNING] Unsafe LSH configuration. A small number of bands or large number of rows increases the chance of false negatives.\n";
                std::cerr << "[WARNING] Consider increasing bands or decreasing rows. (e.g., bands=20, rows=5)\n";
            }
        }

        return config;
    }
}
