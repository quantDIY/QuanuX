#pragma once

#include <duckdb.hpp>
#include <memory>
#include <string>
#include <vector>

namespace QuanuX::ClearStreet {

class DataArchiver {
public:
  DataArchiver(const std::string &dbPath);
  ~DataArchiver();

  // Initialize tables based on mode
  void initSchema(bool l3Mode);

  // Append L1 Quote
  void appendQuote(const std::string &symbol, double bid, double ask,
                   int64_t timestamp);

  // Append L2/L3 Depth Update
  void appendDepth(const std::string &symbol, int level, double price,
                   double size, int side, int64_t timestamp);

  // Append L3 Order
  void appendOrder(const std::string &symbol, const std::string &orderId,
                   double price, double size, int side, int64_t timestamp);

private:
  std::unique_ptr<duckdb::DuckDB> db_;
  std::unique_ptr<duckdb::Connection> conn_;

  // Appenders for high-throughput
  std::unique_ptr<duckdb::Appender> quoteAppender_;
  std::unique_ptr<duckdb::Appender> depthAppender_;
  std::unique_ptr<duckdb::Appender> orderAppender_;
};

} // namespace QuanuX::ClearStreet
