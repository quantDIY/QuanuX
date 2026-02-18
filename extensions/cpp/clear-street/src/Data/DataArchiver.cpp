#include "DataArchiver.hpp"
#include <iostream>

namespace QuanuX::ClearStreet {

DataArchiver::DataArchiver(const std::string &dbPath)
    : db_(nullptr), conn_(nullptr) {
  try {
    db_ = std::make_unique<duckdb::DuckDB>(dbPath);
    conn_ = std::make_unique<duckdb::Connection>(*db_);
  } catch (std::exception &e) {
    std::cerr << "DuckDB Init Error: " << e.what() << std::endl;
  }
}

DataArchiver::~DataArchiver() {
  // Appenders flush on destruction
}

void DataArchiver::initSchema(bool l3Mode) {
  if (!conn_)
    return;

  conn_->Query("CREATE TABLE IF NOT EXISTS quotes (symbol VARCHAR, bid DOUBLE, "
               "ask DOUBLE, ts BIGINT)");

  if (l3Mode) {
    // L3: Market By Order
    conn_->Query(
        "CREATE TABLE IF NOT EXISTS orders_l3 (symbol VARCHAR, order_id "
        "VARCHAR, price DOUBLE, size DOUBLE, side INTEGER, ts BIGINT)");
  } else {
    // L2: Market By Price (Depth)
    conn_->Query(
        "CREATE TABLE IF NOT EXISTS depth_l2 (symbol VARCHAR, level "
        "INTEGER, price DOUBLE, size DOUBLE, side INTEGER, ts BIGINT)");
  }

  try {
    // Appender takes Connection &
    quoteAppender_ = std::make_unique<duckdb::Appender>(*conn_, "quotes");
    if (l3Mode) {
      orderAppender_ = std::make_unique<duckdb::Appender>(*conn_, "orders_l3");
    } else {
      depthAppender_ = std::make_unique<duckdb::Appender>(*conn_, "depth_l2");
    }
  } catch (std::exception &e) {
    std::cerr << "Appender Init Error: " << e.what() << std::endl;
  }
}

void DataArchiver::appendQuote(const std::string &symbol, double bid,
                               double ask, int64_t timestamp) {
  if (!quoteAppender_)
    return;
  try {
    quoteAppender_->BeginRow();
    // DuckDB Append(const char*, len) for strings
    quoteAppender_->Append(symbol.c_str(),
                           static_cast<uint32_t>(symbol.size()));
    quoteAppender_->Append(bid);
    quoteAppender_->Append(ask);
    quoteAppender_->Append(timestamp);
    quoteAppender_->EndRow();
  } catch (...) {
  }
}

void DataArchiver::appendDepth(const std::string &symbol, int level,
                               double price, double size, int side,
                               int64_t timestamp) {
  if (!depthAppender_)
    return;
  try {
    depthAppender_->BeginRow();
    depthAppender_->Append(symbol.c_str(),
                           static_cast<uint32_t>(symbol.size()));
    depthAppender_->Append(level);
    depthAppender_->Append(price);
    depthAppender_->Append(size);
    depthAppender_->Append(side);
    depthAppender_->Append(timestamp);
    depthAppender_->EndRow();
  } catch (...) {
  }
}

void DataArchiver::appendOrder(const std::string &symbol,
                               const std::string &orderId, double price,
                               double size, int side, int64_t timestamp) {
  if (!orderAppender_)
    return;
  try {
    orderAppender_->BeginRow();
    orderAppender_->Append(symbol.c_str(),
                           static_cast<uint32_t>(symbol.size()));
    orderAppender_->Append(orderId.c_str(),
                           static_cast<uint32_t>(orderId.size()));
    orderAppender_->Append(price);
    orderAppender_->Append(size);
    orderAppender_->Append(side);
    orderAppender_->Append(timestamp);
    orderAppender_->EndRow();
  } catch (...) {
  }
}

} // namespace QuanuX::ClearStreet
