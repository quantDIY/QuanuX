#pragma once

#include <string>
#include <functional>
#include <vector>
#include "quanux/annex/TelemetryExhaust.hpp"

namespace quanux {
namespace annex {
namespace enterprise {

/**
 * @brief Zero-allocation Enterprise Redpanda Subscriber.
 * Implements librdkafka C++ bindings to ingest high-throughput telemetry.
 * Completely decoupled from the Read Path (Resolver) for main deployment.
 */
class RedpandaSubscriber {
public:
    using MarketTickCallback = std::function<void(const MarketTick&)>;
    using ExecutionLogCallback = std::function<void(const ExecutionLog&)>;
    using SchemaDriftCallback = std::function<void(const SchemaDriftEvent&)>;

    RedpandaSubscriber(const std::string& brokers, const std::string& group_id);
    ~RedpandaSubscriber();

    void subscribe_market_data(const std::string& topic, MarketTickCallback on_tick);
    void subscribe_executions(const std::string& topic, ExecutionLogCallback on_exec);
    void subscribe_anomalies(const std::string& topic, SchemaDriftCallback on_drift);

    void run_event_loop();
    void stop();

private:
    std::string m_brokers;
    std::string m_group_id;
    bool m_running;

    // Opaque PIMPL pointers to librdkafka structures.
    // Avoids massive librdkafka header inclusion in the main project ABI.
    void* m_kafka_consumer;
    void* m_kafka_topic_partition_list;
};

} // namespace enterprise
} // namespace annex
} // namespace quanux
