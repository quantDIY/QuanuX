#pragma once

#include <string>
#include <functional>
#include <vector>
#include "quanux/annex/TelemetryExhaust.hpp"

namespace quanux {
namespace annex {
namespace do_impl {

/**
 * @brief Zero-allocation NATS JetStream Native Subscriber.
 * Pulls telemetry structures off the wire directly into the RAM-buffer.
 */
class NatsSubscriber {
public:
    using MarketTickCallback = std::function<void(const MarketTick&)>;
    using ExecutionLogCallback = std::function<void(const ExecutionLog&)>;
    using SchemaDriftCallback = std::function<void(const SchemaDriftEvent&)>;

    NatsSubscriber(const std::string& nats_url, const std::string& stream_name);
    ~NatsSubscriber();

    // Subscribe to specific subjects with zero-copy callbacks
    void subscribe_market_data(const std::string& subject, MarketTickCallback on_tick);
    void subscribe_executions(const std::string& subject, ExecutionLogCallback on_exec);
    void subscribe_anomalies(const std::string& subject, SchemaDriftCallback on_drift);

    // Enter the high-performance polling loop
    void run_event_loop();
    void stop();

private:
    std::string m_nats_url;
    std::string m_stream_name;
    bool m_running;
    
    // Opaque pointers to underlying NATS C Client constructs
    void* m_connection;
    void* m_jetstream;
};

} // namespace do_impl
} // namespace annex
} // namespace quanux
