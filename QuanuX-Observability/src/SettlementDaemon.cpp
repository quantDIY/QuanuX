// QuanuX Observability Plane - Settlement Daemon Implementation
// Rule: DuckDB C API only. Exact memory control.

#include "SettlementDaemon.hpp"
#include <format> // C++20 formatting
#include <iostream>

// Assuming flatc generated headers
// #include "../schemas/exchange_registry_generated.h"

namespace QuanuX::Observability {

SettlementDaemon::SettlementDaemon(natsConnection *connection,
                                   const char *duckdb_path)
    : nc_{connection} {

  // Initialize DuckDB via pure C API
  if (duckdb_open(duckdb_path, &db_) == DuckDBError) {
    throw std::runtime_error("Failed to open DuckDB file");
  }
  if (duckdb_connect(db_, &conn_) == DuckDBError) {
    duckdb_close(&db_);
    throw std::runtime_error("Failed to connect to DuckDB");
  }
}

SettlementDaemon::~SettlementDaemon() {
  stop();
  if (conn_)
    duckdb_disconnect(&conn_);
  if (db_)
    duckdb_close(&db_);
}

void SettlementDaemon::start() {
  if (is_running_)
    return;

  // Subscribe to the global Exchange Registry stream over CNATS
  natsStatus s = natsConnection_Subscribe(
      &registry_sub_, nc_, "quanux.registry.state", on_registry_update, this);
  if (s != NATS_OK) {
    throw std::runtime_error("Failed to subscribe to Exchange Registry: " +
                             std::to_string(s));
  }

  is_running_ = true;
  std::cout << "[SettlementDaemon] Active and listening for global epochs.\n";
}

void SettlementDaemon::stop() {
  if (!is_running_)
    return;

  if (registry_sub_) {
    natsSubscription_Unsubscribe(registry_sub_);
    natsSubscription_Destroy(registry_sub_);
    registry_sub_ = nullptr;
  }

  is_running_ = false;
  std::cout << "[SettlementDaemon] Shutting down cleanly.\n";
}

void SettlementDaemon::on_registry_update(natsConnection *nc,
                                          natsSubscription *sub, natsMsg *msg,
                                          void *closure) {
  auto *daemon = static_cast<SettlementDaemon *>(closure);

  // Zero-copy read of the FlatBuffer payload
  const uint8_t *payload = static_cast<const uint8_t *>(natsMsg_GetData(msg));
  // auto registry_state = QuanuX::Registry::GetExchangeRegistryState(payload);

  // Conceptual iteration over exchanges (requires generated flatbuffer header)
  // for (const auto* exchange : *registry_state->exchanges()) {
  //     if (exchange->status() ==
  //     QuanuX::Registry::MarketStatus_FROZEN_SETTLEMENT) {
  //         // SAFESTRING: Use string_view to prevent out-of-bounds reads on
  //         non-null-terminated FlatBuffer strings. std::string_view
  //         exchange_code{exchange->exchange_code()->c_str(),
  //         exchange->exchange_code()->size()};
  //         // Note: handle_settlement_trigger would need to be updated to
  //         accept a string_view
  //         // daemon->handle_settlement_trigger(exchange_code,
  //         exchange->settlement()->settlement_window_sec());
  //     }
  // }

  natsMsg_Destroy(msg);
}

void SettlementDaemon::handle_settlement_trigger(
    std::string_view exchange_code, uint32_t settlement_window_sec) {
  std::cout << std::format("[SettlementDaemon] Epoch Tripped! Freezing {} for "
                           "settlement (Window: {}s).\n",
                           exchange_code, settlement_window_sec);

  flush_to_parquet(exchange_code);
}

void SettlementDaemon::flush_to_parquet(std::string_view exchange_code) {
  // DuckDB C API Parquet Export execution
  // Example logic: Extract buffered execution memory and write to
  // S3/DigitalOcean natively.

  std::string s3_path = std::format(
      "s3://quanux-cold-storage/settlements/{}_latest.parquet", exchange_code);
  std::string query = std::format("COPY (SELECT * FROM memory_buffer WHERE "
                                  "exchange = '{}') TO '{}' (FORMAT PARQUET);",
                                  exchange_code, s3_path);

  duckdb_state state;
  duckdb_result result;

  state = duckdb_query(conn_, query.c_str(), &result);

  if (state == DuckDBError) {
    const char *error_msg = duckdb_result_error(&result);
    // Log via QuanuX system logger
    std::cerr << "[SettlementDaemon] DuckDB Parquet Export FAILED: "
              << (error_msg ? error_msg : "Unknown") << "\n";
  } else {
    std::cout << std::format(
        "[SettlementDaemon] DuckDB successfully pushed {} to {}.\n",
        exchange_code, s3_path);
  }

  duckdb_destroy_result(&result);
}

} // namespace QuanuX::Observability
