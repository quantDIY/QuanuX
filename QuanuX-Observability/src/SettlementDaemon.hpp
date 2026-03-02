// QuanuX Observability Plane - Settlement Daemon Architecture
// Rule: Pulls data triggered by ExchangeRegistry over CNATS, natively writing
// Parquet via DuckDB C API.

#pragma once

#include <duckdb.h>
#include <nats/nats.h>
#include <string>
#include <string_view>
#include <vector>

namespace QuanuX::Observability {

class SettlementDaemon {
public:
  // C++20 initialization
  explicit SettlementDaemon(natsConnection *connection,
                            const char *duckdb_path);
  ~SettlementDaemon();

  SettlementDaemon(const SettlementDaemon &) = delete;
  SettlementDaemon &operator=(const SettlementDaemon &) = delete;

  // Bootstraps CNATS subscriptions and DuckDB connections
  void start();

  // Shuts down cleanly
  void stop();

private:
  // CNATS Async Callback for Exchange Registry Updates
  static void on_registry_update(natsConnection *nc, natsSubscription *sub,
                                 natsMsg *msg, void *closure);

  // Handles the trigger when an exchange hits its FROZEN_SETTLEMENT state
  void handle_settlement_trigger(std::string_view exchange_code,
                                 uint32_t settlement_window_sec);

  // Natively pull data from the hot buffer (conceptualized here) and write to
  // Parquet
  void flush_to_parquet(std::string_view exchange_code);

  natsConnection *nc_{nullptr};
  natsSubscription *registry_sub_{nullptr};

  duckdb_database db_{nullptr};
  duckdb_connection conn_{nullptr};

  bool is_running_{false};
};

} // namespace QuanuX::Observability
