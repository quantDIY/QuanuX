#pragma once
#include <duckdb.hpp>
#include <string>

namespace quanux {
namespace simulator {

class DuckDBFeeder {
public:
  duckdb::DuckDB db_;
  duckdb::Connection conn_;

  DuckDBFeeder(const std::string &path) : db_(path), conn_(db_) {}
};

} // namespace simulator
} // namespace quanux
