#include "stats_engine.h"
#include <chrono>
#include <cmath>
#include <duckdb.hpp>
#include <iostream>
#include <nats.h>
#include <nlohmann/json.hpp>
#include <thread>

using json = nlohmann::json;

// --- Welford's Algorithm for Online Variance ---
void StatsEngine::InstrumentStats::update(double price) {
  count++;
  double delta = price - mean;
  mean += delta / count;
  double delta2 = price - mean;
  M2 += delta * delta2;

  // Window management
  if (window.size() >= window_size) {
    window.erase(window.begin());
  }
  window.push_back(price);
}

double StatsEngine::InstrumentStats::variance() const {
  if (count < 2)
    return 0.0;
  return M2 / (count - 1);
}

double StatsEngine::InstrumentStats::std_dev() const {
  return std::sqrt(variance());
}
// -----------------------------------------------

// -----------------------------------------------

void StatsEngine::CorrelationStats::update(double x, double y) {
  count++;
  double dx = x - mean_x;
  double dy = y - mean_y;
  mean_x += dx / count;
  mean_y += dy / count;
  C_xy += dx * (y - mean_y);
}

double StatsEngine::CorrelationStats::covariance() const {
  if (count < 2)
    return 0.0;
  return C_xy / (count - 1);
}

double StatsEngine::CorrelationStats::correlation(double std_dev_x,
                                                  double std_dev_y) const {
  if (std_dev_x <= 0 || std_dev_y <= 0)
    return 0.0;
  return covariance() / (std_dev_x * std_dev_y);
}

void StatsEngine::update_correlation(const std::string &symbol, double price) {
  // Assumes stats_mutex_ is locked by caller
  for (auto const &[other_sym, other_stats] : stats_map_) {
    if (other_sym == symbol)
      continue;

    // Use last known price (LOCF)
    if (other_stats.window.empty())
      continue;
    double other_price = other_stats.window.back();

    std::string s1 = std::min(symbol, other_sym);
    std::string s2 = std::max(symbol, other_sym);

    double val1 = (s1 == symbol) ? price : other_price;
    double val2 = (s2 == symbol) ? price : other_price;

    corr_map_[{s1, s2}].update(val1, val2);
  }
}

struct StatsEngine::NatsContext {
  natsConnection *conn = nullptr;
  natsSubscription *sub = nullptr;
};

StatsEngine::StatsEngine() : nats_(std::make_unique<NatsContext>()) {}

StatsEngine::~StatsEngine() { stop(); }

void StatsEngine::connect_db(const std::string &path) {
  std::cout << "[Stats] Connecting to DuckDB: " << path << std::endl;
  duckdb::DBConfig config;
  db_ = std::make_unique<duckdb::DuckDB>(path, &config);
  conn_ = std::make_unique<duckdb::Connection>(*db_);

  // Initialize Schema
  conn_->Query("CREATE TABLE IF NOT EXISTS market_snapshots (ts TIMESTAMP, "
               "symbol VARCHAR, price DOUBLE, size DOUBLE)");
}

void StatsEngine::connect_nats(const std::string &url) {
  std::cout << "[Stats] Connecting to NATS: " << url << std::endl;
  natsOptions *opts = nullptr;
  natsOptions_Create(&opts);
  natsOptions_SetURL(opts, url.c_str());

  natsStatus s = natsConnection_Connect(&nats_->conn, opts);
  if (s != NATS_OK) {
    std::cerr << "[Stats] NATS Connect Failed!" << std::endl;
  }
  natsOptions_Destroy(opts);
}

void StatsEngine::run() {
  running_ = true;

  // Subscribe to Market Data
  // Callback lambda wrapper
  auto on_msg = [](natsConnection *nc, natsSubscription *sub, natsMsg *msg,
                   void *closure) {
    StatsEngine *engine = static_cast<StatsEngine *>(closure);
    std::string data = natsMsg_GetData(msg);

    try {
      // 1. Ingest
      auto j = json::parse(data);
      std::string symbol = j.value("symbol", "UNKNOWN");
      double price = j.value("price", 0.0);

      // 2. Persist (DuckDB)
      std::string sql =
          "INSERT INTO market_snapshots VALUES (current_timestamp, '" + symbol +
          "', " + std::to_string(price) + ", 0)";
      engine->conn_->Query(sql);

      // 3. Compute Stats (Welford)
      InstrumentStats *current_stats = nullptr;
      {
        std::lock_guard<std::mutex> lock(engine->stats_mutex_);
        current_stats = &engine->stats_map_[symbol];
        current_stats->update(price);
        engine->update_correlation(symbol, price);
      }

      // 4. Publish Signal (Derived Data)
      if (current_stats && current_stats->count % 10 == 0) {
        json derived;
        derived["symbol"] = symbol;
        derived["volatility"] = current_stats->std_dev();
        derived["mean"] = current_stats->mean;
        derived["z_score"] =
            (current_stats->std_dev() > 0)
                ? (price - current_stats->mean) / current_stats->std_dev()
                : 0.0;

        std::string topic = "STATS." + symbol;
        std::string payload = derived.dump();
        natsConnection_PublishString(engine->nats_->conn, topic.c_str(),
                                     payload.c_str());
      }

    } catch (...) {
    }

    natsMsg_Destroy(msg);
  };

  natsConnection_Subscribe(&nats_->sub, nats_->conn, "MARKET.*", on_msg, this);

  std::cout << "[Stats] Stats Engine Running (Press Ctrl+C to stop)..."
            << std::endl;

  // Keep main thread alive
  while (running_) {
    // Periodic maintenance or deep analysis task could go here
    std::this_thread::sleep_for(std::chrono::seconds(1));

    // Dump correlation matrix every 10s
    static int counter = 0;
    if (++counter % 10 == 0) {
      std::lock_guard<std::mutex> lock(stats_mutex_);
      if (corr_map_.empty())
        continue;
      std::cout << "--- Correlation Matrix ---" << std::endl;
      for (const auto &[pair, stats] : corr_map_) {
        double sdx = stats_map_[pair.first].std_dev();
        double sdy = stats_map_[pair.second].std_dev();
        double rho = stats.correlation(sdx, sdy);
        std::cout << pair.first << "/" << pair.second << ": " << rho
                  << std::endl;
      }
    }
  }
}

void StatsEngine::stop() {
  running_ = false;
  if (nats_->sub)
    natsSubscription_Destroy(nats_->sub);
  if (nats_->conn)
    natsConnection_Destroy(nats_->conn);
}
