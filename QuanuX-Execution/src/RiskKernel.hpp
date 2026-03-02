// QuanuX Execution Plane - The Risk Kernel (Hydration Gate)
// Rule: O(1) Hot Path Execution isolated entirely from network I/O.
// Pessimistic initialization requiring NATS JetStream KV synchronization.

#pragma once

#include <atomic>
#include <cstdint>
#include <iostream>
#include <nats/nats.h>

namespace QuanuX::Execution {

class RiskKernel {
public:
  // C++20 default constructor
  RiskKernel() = default;

  // --- HARD LIMITS (Compiled Physics) ---
  // These are structurally baked into the binary. Memory manipulation over the
  // network cannot alter these.
  static constexpr int64_t MAX_ORDER_QTY =
      100; // Cannot fire more than 100 contracts dynamically.
  static constexpr int64_t MAX_NOTIONAL_PER_ORDER =
      500'000'000; // $5M implied with 2 decimal fixed-point arithmetic if
                   // applicable, or units.

  // --- THE COLD GATE (Pessimistic Global Hydration) ---
  // Must be called upon boot, or post-STONITH takeover.
  // Blocks execution until the JetStream KV global state seeds local RAM.
  bool hydrate_from_mesh(natsConnection *nc, const char *kv_bucket) {

    jsCtx *js_ctx = nullptr;
    jsErrCode jerr;
    natsStatus s = natsConnection_JetStream(&js_ctx, nc, nullptr);
    if (s != NATS_OK) {
      std::cerr
          << "[RiskKernel] FATAL: Failed to context JetStream for Hydration.\n";
      return false;
    }

    jsKVStore *kv = nullptr;
    s = js_KeyValue(&kv, js_ctx, kv_bucket);
    if (s != NATS_OK) {
      std::cerr << "[RiskKernel] FATAL: Failed to access JetStream KV Bucket: "
                << kv_bucket << "\n";
      return false;
    }

    jsKVEntry *entry = nullptr;
    // Synchronous, blocking read to acquire global execution state.
    s = jsKVStore_Get(&entry, kv, "global.notional.exposure");

    if (s == NATS_OK) {
      // Reconstruct the 64-bit integer from binary payload
      const void *data = jsKVEntry_Value(entry);
      int len = jsKVEntry_ValueLen(entry);

      if (len == sizeof(int64_t)) {
        current_global_notional_ = *reinterpret_cast<const int64_t *>(data);
        std::cout << "[RiskKernel] HYDRATION SUCCESS: Synced "
                  << current_global_notional_ << " from global mesh.\n";
      } else {
        std::cerr
            << "[RiskKernel] FATAL: KV Entry size mismatch. Data corrupted.\n";
        jsKVEntry_Destroy(entry);
        jsKVStore_Destroy(kv);
        return false;
      }
      jsKVEntry_Destroy(entry);
    } else if (s == NATS_NOT_FOUND) {
      // Initial cluster boot state
      current_global_notional_ = 0;
      std::cout << "[RiskKernel] HYDRATION SUCCESS: Clean slate. Initializing "
                   "to zero.\n";
    } else {
      std::cerr << "[RiskKernel] FATAL: KV Store read failure.\n";
      jsKVStore_Destroy(kv);
      return false;
    }

    // Hydration Complete. Unlocking the Sovereign Engine.
    is_hot_.store(true, std::memory_order_relaxed);

    jsKVStore_Destroy(kv);
    return true;
  }

  // --- THE HOT PATH (Optimistic Local Execution) ---
  // Runs per-packet prior to matching engine TCP write.
  // O(1) inline execution. Zero network syscalls. Zero database queries. Zero
  // memory locks.
  [[nodiscard]] inline bool check_trade(int64_t qty, int64_t price) noexcept {
    // Guard: If we are isolated, dead, or unhydrated, drop the trade instantly.
    if (!is_hot_.load(std::memory_order_relaxed)) {
      return false;
    }

    // 1. Enforce compiled limits
    if (qty > MAX_ORDER_QTY) {
      return false;
    }

    // 2. Compute local cost
    const int64_t cost = qty * price;

    if (cost > MAX_NOTIONAL_PER_ORDER) {
      return false;
    }

    // 3. Enforce dynamic hydrated limits against local state
    if ((current_global_notional_ + cost) >
        daily_notional_cap_.load(std::memory_order_relaxed)) {
      return false;
    }

    // 4. Optimistically increment the local RAM matrix.
    // (Asynchronous telemetry handles eventual consistency broadcasting back to
    // the mesh)
    current_global_notional_ += cost;

    return true;
  }

  // --- ASYNC CONTROL ---
  // Called by background threads mapping the quanux.control.risk.cap NATS
  // subject.
  void set_daily_notional_cap(int64_t new_cap) noexcept {
    daily_notional_cap_.store(new_cap, std::memory_order_relaxed);
  }

private:
  // --- SOFT LIMITS (Local RAM Matrix) ---
  // Memory alignment: largest types first to prevent compiler padding.
  // Seeded by Hydration, drifted optimistically by local execution.
  int64_t current_global_notional_{0};
  std::atomic<int64_t> daily_notional_cap_{
      10'000'000'000}; // E.g., $100M with 2 assumed decimal places

  // By default, the node is completely isolated and dead to execution.
  std::atomic<bool> is_hot_{false};
};

} // namespace QuanuX::Execution
