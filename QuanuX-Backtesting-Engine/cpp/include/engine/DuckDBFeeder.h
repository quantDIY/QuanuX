#pragma once

#include "duckdb.hpp"
#include "engine/metrics/AdvancedStatsAnalyzer.h"
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
      auto res = conn_.Query("CREATE TABLE IF NOT EXISTS crucible_trades ("
                             "trade_id BIGINT, strategy_id VARCHAR, "
                             "entry_time BIGINT, exit_time BIGINT, "
                             "direction VARCHAR, size INTEGER, "
                             "entry_price DOUBLE, exit_price DOUBLE, "
                             "slippage_bps DOUBLE, mae DOUBLE, mfe DOUBLE, "
                             "queue_position INTEGER, profit DOUBLE)");
      if (res->HasError())
        std::cerr << "Table Create Error: " << res->GetError() << std::endl;

      trade_appender_ =
          std::make_unique<duckdb::Appender>(conn_, "crucible_trades");
    } catch (std::exception &e) {
      std::cerr << "DuckDB Appender Init Error: " << e.what() << std::endl;
    }
  }

  std::string get_metrics_json(const std::string &strategy_id) {
    if (trade_appender_) {
      trade_appender_->Flush();
    }
    std::string query =
        "SELECT sum(profit) as total_profit, count(*) as total_trades, "
        "avg(slippage_bps) as avg_slippage, "
        "avg(mae) as avg_mae, avg(mfe) as avg_mfe "
        "FROM crucible_trades WHERE strategy_id = '" +
        strategy_id + "'";
    auto result = conn_.Query(query);
    if (result->HasError()) {
      return "{\"error\": \"" + result->GetError() + "\"}";
    }

    if (result->RowCount() == 0) {
      return "{\"total_profit\": 0, \"total_trades\": 0}";
    }

    double profit = result->GetValue(0, 0).IsNull()
                        ? 0.0
                        : result->GetValue(0, 0).GetValue<double>();
    int64_t trades = result->GetValue(1, 0).IsNull()
                         ? 0
                         : result->GetValue(1, 0).GetValue<int64_t>();
    double slippage = result->GetValue(2, 0).IsNull()
                          ? 0.0
                          : result->GetValue(2, 0).GetValue<double>();
    double mae = result->GetValue(3, 0).IsNull()
                     ? 0.0
                     : result->GetValue(3, 0).GetValue<double>();
    double mfe = result->GetValue(4, 0).IsNull()
                     ? 0.0
                     : result->GetValue(4, 0).GetValue<double>();

    return "{"
           "\"strategy_id\": \"" +
           strategy_id +
           "\", "
           "\"total_profit\": " +
           std::to_string(profit) +
           ", "
           "\"total_trades\": " +
           std::to_string(trades) +
           ", "
           "\"avg_slippage_bps\": " +
           std::to_string(slippage) +
           ", "
           "\"avg_mae\": " +
           std::to_string(mae) +
           ", "
           "\"avg_mfe\": " +
           std::to_string(mfe) + "}";
  }

  std::string get_metrics_json_advanced(const std::string &strategy_id,
                                        int mc_iterations = 1000) {
    if (trade_appender_) {
      trade_appender_->Flush();
    }

    std::string query =
        "SELECT trade_id, strategy_id, entry_time, exit_time, direction, size, "
        "entry_price, exit_price, slippage_bps, mae, mfe, queue_position, "
        "profit FROM crucible_trades WHERE strategy_id = '" +
        strategy_id + "' ORDER BY entry_time ASC";

    auto result = conn_.Query(query);
    if (result->HasError()) {
      return "{\"error\": \"" + result->GetError() + "\"}";
    }

    std::vector<metrics::CrucibleTrade> trades;
    trades.reserve(result->RowCount());

    for (idx_t r = 0; r < result->RowCount(); r++) {
      metrics::CrucibleTrade t{};
      t.entry_time_ns = result->GetValue(2, r).IsNull()
                            ? 0
                            : result->GetValue(2, r).GetValue<int64_t>() * 1000;
      t.exit_time_ns = result->GetValue(3, r).IsNull()
                           ? 0
                           : result->GetValue(3, r).GetValue<int64_t>() * 1000;
      t.profit = result->GetValue(12, r).IsNull()
                     ? 0.0
                     : result->GetValue(12, r).GetValue<double>();
      trades.push_back(t);
    }

    metrics::AdvancedStatsResult adv =
        metrics::AdvancedStatsAnalyzer::compute(trades, mc_iterations);

    return "{"
           "\"strategy_id\": \"" +
           strategy_id +
           "\", "
           "\"win_rate\": " +
           std::to_string(adv.win_rate) +
           ", "
           "\"avg_win\": " +
           std::to_string(adv.avg_win) +
           ", "
           "\"avg_loss\": " +
           std::to_string(adv.avg_loss) +
           ", "
           "\"kelly_fraction\": " +
           std::to_string(adv.kelly_fraction) +
           ", "
           "\"monte_carlo_expected_pnl\": " +
           std::to_string(adv.mc_expected_pnl) +
           ", "
           "\"monte_carlo_95_percent_low\": " +
           std::to_string(adv.mc_95_percentile_low_pnl) +
           ", "
           "\"max_drawdown_dollars\": " +
           std::to_string(adv.max_drawdown_dollars) +
           ", "
           "\"max_drawdown_percent\": " +
           std::to_string(adv.max_drawdown_percent) +
           ", "
           "\"uncertainty_adjusted_kelly\": " +
           std::to_string(adv.uncertainty_adjusted_kelly) + "}";
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
