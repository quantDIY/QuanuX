#pragma once

#include "memory_pool.hpp"
#include "price_matrix.hpp"
#include "quanux/MarketTick.hpp"
#include "quanux/SPSCQueue.hpp"
#include "quanux/sovereign_state.hpp"
#include "spreader/clock_provider.hpp"
#include <memory>

// Deep injection: The compiler physically copies the strategy header here
#include INJECTED_STRATEGY_HEADER

static_assert(sizeof(quanux::spreader::strategy::StrategyState) <= 64,
              "SHM/DMA Handshake Risk: StrategyState exceeds L1 cache line "
              "size (64 bytes).");

#include <atomic>
#include <immintrin.h>
#include <thread>

// Forward delcaration for cnats missing from header to avoid pollution
struct __natsConnection;
typedef struct __natsConnection natsConnection;

struct __natsSubscription;
typedef struct __natsSubscription natsSubscription;

struct __natsMsg;
typedef struct __natsMsg natsMsg;

namespace quanux::spreader {

/**
 * @brief The High-Frequency Event Payload triggered by the Innode Math.
 * Cache-aligned to prevent false sharing with the Producer thread.
 */
struct alignas(64) SpreaderEvent {
  uint64_t trigger_ts;
  // Direct pointer-stable view of the tick in the MemoryPool
  const quanux::MarketTick *triggering_tick;
  // Explicit padding to 64 bytes
  uint8_t _pad[48];
};

static_assert(sizeof(SpreaderEvent) == 64,
              "SpreaderEvent must be exactly 64 bytes");

class DualThreadSpreader {
public:
  DualThreadSpreader();
  ~DualThreadSpreader();

  void start();
  void stop();

  // Explicit NATS Callback Hook
  void handle_market_tick(natsMsg *msg);

private:
  void innode_pipe_loop();
  void strategy_fix_loop();

  std::atomic<bool> running_;

  // Core Communication
  quanux::SPSCQueue<SpreaderEvent> event_queue_;

  // Injected Strategy Context & Math State
  strategy::StrategyState strategy_state_;

  // HFT LOCF Price Cache
  PriceMatrix<8192> price_matrix_;

  // L3 Hardware Interlock Memory Map
  quanux::SovereignState *sovereign_state_{nullptr};

  // Zero-Jitter Static Allocators
  MemoryPool<quanux::MarketTick, 8192> tick_pool_;
  MemoryPool<SpreaderEvent, 1024> event_pool_;

  std::thread producer_thread_;
  std::thread consumer_thread_;

  // Sim-Live Parity
  std::unique_ptr<ClockProvider> clock_;

  natsConnection *nc_{nullptr};
  natsSubscription *sub_{nullptr};
};

} // namespace quanux::spreader
