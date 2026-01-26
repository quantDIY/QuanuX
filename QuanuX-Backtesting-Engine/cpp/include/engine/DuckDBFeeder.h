#pragma once

#include "duckdb.hpp"
#include <functional>
#include <iostream>
#include <vector>

namespace quanux::engine {

class DuckDBFeeder {
public:
  duckdb::DuckDB db_;
  duckdb::Connection conn_;

  DuckDBFeeder(const std::string &db_path = ":memory:")
      : db_(db_path.empty() ? nullptr : db_path.c_str()), conn_(db_) {
    if (db_path == ":memory:") {
      std::cout << "DuckDB: Initialized in-memory database." << std::endl;
    } else {
      std::cout << "DuckDB: Connected to " << db_path << std::endl;
    }
  }

  void load_parquet(const std::string &glob_path,
                    const std::string &table_name) {
    std::string query = "CREATE TABLE " + table_name +
                        " AS SELECT * FROM read_parquet('" + glob_path + "');";
    auto result = conn_.Query(query);
    if (result->HasError()) {
      std::cerr << "DuckDB Error loading parquet: " << result->GetError()
                << std::endl;
    } else {
      std::cout << "DuckDB: Loaded " << glob_path << " into " << table_name
                << std::endl;
    }
  }

  // Mock method to stream ticks - in production we use Arrow pointers
  void stream_ticks(
      const std::string &table_name,
      std::function<void(uint64_t, int64_t, uint32_t, bool, bool)> callback) {
    auto result = conn_.Query("SELECT * FROM " + table_name + " LIMIT 10");
    if (result->HasError()) {
      std::cerr << "Query Error: " << result->GetError() << std::endl;
      return;
    }
    // Iterate not implemented in mock
    std::cout << "DuckDB: Would stream " << result->RowCount() << " rows..."
              << std::endl;
  }
};
} // namespace quanux::engine
