#pragma once
#include <string>
#include <vector>

namespace quanux {
namespace annex {
namespace lookback {

// Mock representation of an Arrow/DuckDB dataset
struct LookbackResultMatrix {
    std::vector<std::string> columns;
    std::vector<std::vector<std::string>> rows;
};

class MarkdownFormatter {
public:
    static std::string formatToMarkdown(const LookbackResultMatrix& matrix);
};

} // namespace lookback
} // namespace annex
} // namespace quanux
