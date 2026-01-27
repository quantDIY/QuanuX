#pragma once
#include <Eigen/Dense>
#include <atomic>
#include <map>
#include <memory>
#include <mutex>
#include <string>
#include <utility>
#include <vector>

// Forward decls
namespace duckdb {
class Connection;
class DuckDB;
} // namespace duckdb

class StatsEngine {
public:
  StatsEngine();
  ~StatsEngine();

  void connect_nats(const std::string &url);
  void connect_db(const std::string &path);
  void run();
  void stop();

private:
  std::atomic<bool> running_{false};
  std::unique_ptr<duckdb::DuckDB> db_;
  std::unique_ptr<duckdb::Connection> conn_;

  // NATS handle (void* to keep header clean of C includes if desired, but
  // sticking to simple for now) We'll impl in cpp
  struct NatsContext;
  std::unique_ptr<NatsContext> nats_;

  // Online Stats State (Example: Keeping a rolling window for correlation)
  // Vector of recent prices for correlation calculation
  struct InstrumentStats {
    uint64_t count = 0;
    double mean = 0.0;
    double M2 = 0.0; // Welford's algorithm accumulator

    // Z-Score window
    std::vector<double> window;
    size_t window_size = 100;

    void update(double price);
    double variance() const;
    double std_dev() const;
  };

  // Online Correlation State (Pairwise)
  struct CorrelationStats {
    uint64_t count = 0;
    double mean_x = 0.0;
    double mean_y = 0.0;
    double C_xy = 0.0; // Co-moment accumulator

    void update(double x, double y);
    double covariance() const;
    double correlation(double std_dev_x, double std_dev_y) const;
  };

  // Map instrument_id (symbol) -> Stats
  std::map<std::string, InstrumentStats> stats_map_;

  // Map (symbolA, symbolB) -> CorrelationStats
  // Key is sorted: pair(min(A,B), max(A,B))
  std::map<std::pair<std::string, std::string>, CorrelationStats> corr_map_;
  std::mutex
      stats_mutex_; // Protect maps from NATS callbacks if multithreaded (NATS C
                    // is usually single threaded callback context, but safer)

  void update_correlation(const std::string &symbol, double price);
};
