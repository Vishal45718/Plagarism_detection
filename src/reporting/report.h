#ifndef REPORT_H
#define REPORT_H

#include <string>
#include <vector>
#include "../similarity/similarity.h"
#include "../cli/cli.h"

namespace reporting {

    struct PairMatch {
        std::string file1;
        std::string file2;
        similarity::MatchResult metrics;
        double primary_score; // used for sorting (Top-N)
    };

    void generate_json_report(const std::vector<PairMatch>& matches,
                              const cli::Config& config,
                              int total_files,
                              int total_comparisons);

    void generate_html_report(const std::vector<PairMatch>& matches,
                              const cli::Config& config,
                              int total_files,
                              int total_comparisons);
}

#endif
