#include "spreader/engine.hpp"
#include <cstring>
#include <iostream>
#include <nats.h>

namespace quanux::spreader {

// The raw NATS callback running inside the C library's dispatch thread
// In an HFT environment, this dispatch thread is the true "Innode Data Pipe"
static void on_nats_message(natsConnection *nc, natsSubscription *sub,
                            natsMsg *msg, void *closure) {
  (void)nc;
  (void)sub;
  auto *spreader = static_cast<DualThreadSpreader *>(closure);
  spreader->handle_market_tick(msg);
  natsMsg_Destroy(msg);
}

DualThreadSpreader::DualThreadSpreader()
    : running_(false), event_queue_(1024) {}

DualThreadSpreader::~DualThreadSpreader() { stop(); }

void DualThreadSpreader::start() {
  running_ = true;
  strategy::ActiveStrategy::init(strategy_state_);

  // NATS configuration
  natsOptions *opts = nullptr;
  natsOptions_Create(&opts);
  // In production this URL would be injected, defaulting to localhost for dev
  natsOptions_SetURL(opts, NATS_DEFAULT_URL);

  natsStatus s = natsConnection_Connect(&nc_, opts);
  if (s == NATS_OK) {
    std::cout << "[Spreader] Connected to NATS DMA pipe." << std::endl;
  } else {
    std::cerr << "[Spreader] FAILED TO CONNECT TO NATS." << std::endl;
  }
  natsOptions_Destroy(opts);

  producer_thread_ = std::thread(&DualThreadSpreader::innode_pipe_loop, this);
  consumer_thread_ = std::thread(&DualThreadSpreader::strategy_fix_loop, this);
}

void DualThreadSpreader::stop() {
  running_ = false;

  if (sub_) {
    natsSubscription_Destroy(sub_);
    sub_ = nullptr;
  }
  if (nc_) {
    natsConnection_Destroy(nc_);
    nc_ = nullptr;
  }

  if (producer_thread_.joinable())
    producer_thread_.join();
  if (consumer_thread_.joinable())
    consumer_thread_.join();
}

/**
 * @brief Zero-Copy Wire-to-Math Pipe Event Handler
 * Executes the entire HFT data-ingest pipeline within a single CPU cycle path.
 */
[[gnu::always_inline]] void
DualThreadSpreader::handle_market_tick(natsMsg *msg) {
  const int msg_len = natsMsg_GetDataLength(msg);
  if (msg_len != sizeof(quanux::MarketTick))
    return; // Integrity check

  // 1. NATS Zero-Copy: Direct 64-byte structural view over the socket buffer
  const auto *raw_tick =
      reinterpret_cast<const quanux::MarketTick *>(natsMsg_GetData(msg));

  // 2. The MemoryPool Pivot: SIMD-optimized deep copy into our L1 stable arena
  quanux::MarketTick *active_tick = tick_pool_.next_slot();
  std::memcpy(active_tick, raw_tick, sizeof(quanux::MarketTick));

  // 3. The One-Pass Update: Inject into Price Matrix array for O(1) LOCF lookup
  // We capture the low 32-bits of the TSC clock for the zero-overhead telemetry
  // heartbeat
  uint32_t tsc_pulse = static_cast<uint32_t>(__builtin_ia32_rdtsc());
  price_matrix_.update_price(active_tick->instrument_id, active_tick->price,
                             tsc_pulse);

  // 4. Trigger Math & Update L3 Interlock Tap
  if (strategy::ActiveStrategy::calculate(strategy_state_, *active_tick)) {
    if (sovereign_state_) {
      // Update Telemetry Tap for Visual Record and Core 5 Sentinel Vigil
      uint8_t idx = sovereign_state_->tap_index.load(std::memory_order_relaxed);
      auto &tap = sovereign_state_->telemetry_tap[idx % 3];
      tap.best_bid = active_tick->price;
      tap.best_ask = active_tick->price + 0.5; // Stub ask
      tap.alpha = 1.0;
      tap.tsc_lo = tsc_pulse;
      sovereign_state_->tap_index.store(idx + 1, std::memory_order_relaxed);
    }

    SpreaderEvent *event = event_pool_.next_slot();
    event->trigger_ts = __builtin_ia32_rdtsc();
    event->triggering_tick = active_tick; // Stable pointer from MemoryPool

    event_queue_.push(*event);
  }
}

void DualThreadSpreader::innode_pipe_loop() {
  // We bind the subscription within the thread loop to ensure the NATS library
  // considers this our primary working context, though NATS dispatches
  // internally.
  if (nc_) {
    natsConnection_Subscribe(&sub_, nc_, "MARKET.BIN", on_nats_message, this);
    std::cout << "[Thread 1] Innode Data Pipe Started. Spinning on MARKET.BIN."
              << std::endl;
  }

  while (running_) {
    // Keeps the L1 cache warm and CPU execution port active on this physical
    // core
    _mm_pause();
  }
}

void DualThreadSpreader::strategy_fix_loop() {
  std::cout << "[Thread 2] Strategy & FIX Order Entry Started." << std::endl;
  SpreaderEvent event;

  while (running_) {
    // Spin-wait on SPSC lock-free queue in nanoseconds
    if (event_queue_.pop(event)) {
      // [EVENT CAUGHT]
      // We have a direct pointer-stable view of the tick via
      // event.triggering_tick

      // Assume we identify Leg B offset index for this spread (e.g. index 1)
      // Pre-Flight Check: Refresh the Leg B offset price before firing the FIX
      // message
      double leg_b_price = price_matrix_.get_price(1);
      uint32_t leg_b_seq = price_matrix_.get_seq(1);

      (void)leg_b_price;
      (void)leg_b_seq;

      // [MOVEMENT I: THE L3 CMP CHECK]
      // Inject a single assembly CMP check checking the L3 risk_interlock
      // before every outbound packet
      if (sovereign_state_ && sovereign_state_->risk_interlock.load(
                                  std::memory_order_relaxed) != 0) {
        // Hardware interlock triggered. Do NOT fire.
        continue;
      }

      // Execute FIX connection payload here... -> fire()
    } else {
      _mm_pause(); // Keep L1 Cache warm without yielding CPU scheduler
    }
  }
}

} // namespace quanux::spreader
