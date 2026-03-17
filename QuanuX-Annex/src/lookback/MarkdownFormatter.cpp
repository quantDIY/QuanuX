#include "quanux/annex/lookback/MarkdownFormatter.hpp"
#include <sstream>
#include <iostream>

namespace quanux {
namespace annex {
namespace lookback {

std::string MarkdownFormatter::formatToMarkdown(const LookbackResultMatrix& matrix) {
    if (matrix.columns.empty()) return "";

    std::ostringstream md;
    
    // Header Row
    md << "| ";
    for (const auto& col : matrix.columns) {
        md << col << " | ";
    }
    md << "\n";
    
    // Separator Row
    md << "|";
    for (size_t i = 0; i < matrix.columns.size(); ++i) {
        md << "---|";
    }
    md << "\n";
    
    // Data Rows
    for (const auto& row : matrix.rows) {
        md << "| ";
        for (const auto& cell : row) {
            md << cell << " | ";
        }
        md << "\n";
    }
    
    std::string result = md.str();
    std::cout << "[LOOKBACK] Response Formatter converted Arrow Matrix to AI-digestible Markdown (" << result.size() << " bytes)." << std::endl;
    return result;
}

} // namespace lookback
} // namespace annex
} // namespace quanux
