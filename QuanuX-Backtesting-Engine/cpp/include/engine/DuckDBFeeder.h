#pragma once

#include "duckdb.hpp"
#include "engine/metrics/PerformanceAnalyzer.h"
#include <functional>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace quanux::engine {

class DuckDBFeeder {
public:
  duckdb::DuckDB db_;
  duckdb::Connection conn_;
  std::unique_ptr<duckdb::Appender> trade_appender_;
  std::unique_ptr<duckdb::Appender> perf_appender_;

  DuckDBFeeder(const std::string &db_path = ":memory:")
      : db_(db_path.empty() ? nullptr : db_path.c_str()), conn_(db_) {
    if (db_path == ":memory:") {
      std::cout << "DuckDB: Initialized in-memory database." << std::endl;
    } else {
      std::cout << "DuckDB: Connected to " << db_path << std::endl;
    }
  }

  void init_appenders() {
    try {
      trade_appender_ =
          std::make_unique<duckdb::Appender>(conn_, "crucible_trades");
      // perf_appender_ = std::make_unique<duckdb::Appender>(conn_,
      // "crucible_performance");
    } catch (std::exception &e) {
      std::cerr << "DuckDB Appender Init Error: " << e.what() << std::endl;
    }
  }

  void append_trades(const std::string &strategy_id,
                     const std::vector<metrics::CrucibleTrade> &trades) {
    if (!trade_appender_)
      return;

    // Write aligned memory blocks row by row
    // Bypass all SQL query planners and parsers
    try {
      uint64_t trade_id = 0;
      for (const auto &t : trades) {
        trade_appender_->BeginRow();
        trade_appender_->Append(trade_id++);
        trade_appender_->Append(strategy_id.c_str(),
                                static_cast<uint32_t>(strategy_id.size()));

        // entry_time and exit_time (TIMESTAMP expects int64_t micros in DuckDB)
        trade_appender_->Append(static_cast<int64_t>(t.entry_time_ns / 1000));
        trade_appender_->Append(static_cast<int64_t>(t.exit_time_ns / 1000));

        // direction
        std::string dir = t.isLong ? "LONG" : "SHORT";
        trade_appender_->Append(dir.c_str(), static_cast<uint32_t>(dir.size()));

        // size
        trade_appender_->Append(static_cast<int32_t>(t.size));

        // prices
        trade_appender_->Append(t.entryPrice);
        trade_appender_->Append(t.exitPrice);
        trade_appender_->Append(t.latency_slippage_bps);
        trade_appender_->Append(t.mae);
        trade_appender_->Append(t.mfe);
        trade_appender_->Append(
            static_cast<int32_t>(t.queue_position_at_entry));
        trade_appender_->Append(t.profit);

        trade_appender_->EndRow();
      }

      // Flush to disk/RAM
      trade_appender_->Flush();
      std::cout << "[DuckDBAppender] Flushed " << trades.size()
                << " aligned CrucibleTrades to DB." << std::endl;
    } catch (std::exception &e) {
      std::cerr << "DuckDB Appender Write Error: " << e.what() << std::endl;
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
    std::cout << "DuckDB: Would stream " << result->RowCount() << " rows..."
              << std::endl;
  }
};
} // namespace quanux::engine
