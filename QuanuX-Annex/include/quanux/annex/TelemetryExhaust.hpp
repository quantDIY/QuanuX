#pragma once

#include <cstdint>

namespace quanux {
namespace annex {

#pragma pack(push, 1)

/**
 * @brief Zero-latency struct for Market Tick (L2/L3 data) buffers.
 */
struct MarketTick {
    uint64_t timestamp_ns;
    uint32_t instrument_id;
    uint32_t venue_id;         // Annex Identity Projection
    uint32_t route_id;         // Annex Identity Projection
    uint32_t counterparty_id;  // Annex Identity Projection
    double bid_price;
    double ask_price;
    uint32_t bid_size;
    uint32_t ask_size;
    uint8_t level; // 2 for L2, 3 for L3
};

/**
 * @brief Execution log including trade fills and granular latencies.
 */
struct ExecutionLog {
    uint64_t order_id;
    uint64_t execution_timestamp_ns;
    double fill_price;
    uint32_t fill_quantity;
    uint32_t strategy_id;
    uint32_t latency_micros; // Wire-to-kernel back-propagation latency
};

/**
 * @brief Drift event for capturing FIX Tag 99999 anomalies
 */
struct SchemaDriftEvent {
    uint64_t event_timestamp_ns;
    uint32_t connection_id;
    uint32_t anomaly_code;
    char fix_payload_snippet[128]; // Raw snippet containing tag 99999 payload
};

#pragma pack(pop)

} // namespace annex
} // namespace quanux
