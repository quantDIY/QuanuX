#pragma once
#include <string>

namespace quanux {
namespace annex {
namespace lookback {

class QueryTranslator {
public:
    static std::string translateGraphQLToDuckDB(const std::string& graphqlIntent);
};

} // namespace lookback
} // namespace annex
} // namespace quanux
