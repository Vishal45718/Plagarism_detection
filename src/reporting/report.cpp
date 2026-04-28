#include "report.h"
#include "../utils/file_utils.h"
#include <iostream>
#include <fstream>
#include <iomanip>

namespace reporting {

    void generate_json_report(const std::vector<PairMatch>& matches,
                              const cli::Config& config,
                              int total_files,
                              int total_comparisons) {
        std::cout << "{\n";
        std::cout << "  \"metadata\": {\n";
        std::cout << "    \"total_files\": " << total_files << ",\n";
        std::cout << "    \"total_comparisons\": " << total_comparisons << ",\n";
        std::cout << "    \"threshold_used\": " << config.threshold << ",\n";
        std::cout << "    \"kgram\": " << config.kgram << ",\n";
        std::cout << "    \"window_thresh\": " << config.window_thresh << "\n";
        std::cout << "  },\n";
        std::cout << "  \"matches\": [\n";

        for (size_t i = 0; i < matches.size(); ++i) {
            const auto& m = matches[i];
            std::cout << "    {\n";
            std::cout << "      \"file1\": \"" << utils::escape_json(m.file1) << "\",\n";
            std::cout << "      \"file2\": \"" << utils::escape_json(m.file2) << "\",\n";
            std::cout << "      \"jaccard_similarity\": " << std::fixed << std::setprecision(4) << m.metrics.jaccard_similarity << ",\n";
            std::cout << "      \"cosine_similarity\": " << std::fixed << std::setprecision(4) << m.metrics.cosine_similarity << ",\n";
            std::cout << "      \"containment\": " << std::fixed << std::setprecision(4) << m.metrics.containment << ",\n";
            std::cout << "      \"common_hashes\": " << m.metrics.common_hashes << "\n";
            std::cout << "    }" << (i + 1 == matches.size() ? "" : ",") << "\n";
        }
        std::cout << "  ]\n}\n";
    }

    void generate_html_report(const std::vector<PairMatch>& matches,
                              const cli::Config& config,
                              int total_files,
                              int total_comparisons) {
        if (config.output_html.empty()) return;

        std::ofstream out(config.output_html);
        if (!out.is_open()) {
            std::cerr << "Warning: Could not open " << config.output_html << " for writing HTML report.\n";
            return;
        }

        out << "<!DOCTYPE html>\n<html lang=\"en\">\n<head>\n";
        out << "<meta charset=\"UTF-8\">\n<title>Plagiarism Report</title>\n";
        out << "<style>\n";
        out << "body { font-family: Arial, sans-serif; margin: 20px; background: #f9f9f9; }\n";
        out << "h1 { color: #333; }\n";
        out << ".card { background: #fff; padding: 15px; margin-bottom: 20px; border-radius: 5px; box-shadow: 0 2px 5px rgba(0,0,0,0.1); }\n";
        out << "table { width: 100%; border-collapse: collapse; }\n";
        out << "th, td { text-align: left; padding: 8px; border-bottom: 1px solid #ddd; }\n";
        out << "th { background-color: #f2f2f2; }\n";
        out << "</style>\n</head>\n<body>\n";
        
        out << "<h1>Plagiarism Detection Report</h1>\n";
        out << "<div class=\"card\">\n";
        out << "<h2>Summary</h2>\n";
        out << "<p>Total files processed: " << total_files << "</p>\n";
        out << "<p>Total comparisons: " << total_comparisons << "</p>\n";
        out << "<p>Matches found: " << matches.size() << "</p>\n";
        out << "</div>\n";

        out << "<div class=\"card\">\n";
        out << "<h2>Matches</h2>\n";
        out << "<table>\n";
        out << "<tr><th>File 1</th><th>File 2</th><th>Jaccard</th><th>Cosine</th><th>Containment</th></tr>\n";
        
        auto format_lines = [](const std::vector<int>& lines) {
            if (lines.empty()) return std::string("None");
            std::string res;
            int start = lines[0], end = lines[0];
            for (size_t i = 1; i <= lines.size(); ++i) {
                if (i < lines.size() && lines[i] == end + 1) {
                    end = lines[i];
                } else {
                    if (start == end) res += std::to_string(start) + ", ";
                    else res += std::to_string(start) + "-" + std::to_string(end) + ", ";
                    if (i < lines.size()) { start = lines[i]; end = lines[i]; }
                }
            }
            if (!res.empty()) res.resize(res.size() - 2);
            return res;
        };

        for (const auto& m : matches) {
            out << "<tr>";
            out << "<td>" << m.file1 << "<br><small>Lines: " << format_lines(m.metrics.matched_lines_file1) << "</small></td>";
            out << "<td>" << m.file2 << "<br><small>Lines: " << format_lines(m.metrics.matched_lines_file2) << "</small></td>";
            out << "<td>" << std::fixed << std::setprecision(4) << m.metrics.jaccard_similarity << "</td>";
            out << "<td>" << std::fixed << std::setprecision(4) << m.metrics.cosine_similarity << "</td>";
            out << "<td>" << std::fixed << std::setprecision(4) << m.metrics.containment << "</td>";
            out << "</tr>\n";
        }
        
        out << "</table>\n";
        out << "</div>\n";
        out << "</body>\n</html>\n";
    }
}
