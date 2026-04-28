#ifndef CLI_H
#define CLI_H

#include <string>
#include <optional>

namespace cli {

    enum class Mode {
        SENSITIVE,
        BALANCED,
        STRICT
    };

    enum class Metric {
        JACCARD,
        COSINE,
        BOTH
    };

    struct Config {
        std::string target_dir;
        double threshold = 0.1;
        int kgram = 15;
        int window_thresh = 20;
        bool verbose = false;
        int top_n = -1;
        Mode mode = Mode::BALANCED;
        Metric metric = Metric::JACCARD;
        bool recursive = false;
        std::string output_html = "";
        bool valid = true;
        std::string error_msg = "";
    };

    Config parse_args(int argc, char** argv);
    void print_help();
}

#endif // CLI_H
