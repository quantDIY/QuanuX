#pragma once
#include <vector>
#include <cstdint>
#include <memory>
#include <mutex>
#include <thread>
#include <atomic>
#include <string>

// NATS C Client
#include <nats/nats.h>

namespace quanux {
namespace annex {

// Aligned C++ Struct-of-Arrays (SoA) memory buffers
struct alignas(64) WarmSoABuffer {
    std::vector<int64_t> timestamp_ns;
    std::vector<int32_t> instrument_id;
    std::vector<int32_t> venue_id;
    std::vector<int32_t> route_id;
    std::vector<int32_t> counterparty_id;
    std::vector<float> bid;
    std::vector<float> ask;
    std::vector<int32_t> volume;
    
    void reserve(size_t capacity) {
        timestamp_ns.reserve(capacity);
        instrument_id.reserve(capacity);
        venue_id.reserve(capacity);
        route_id.reserve(capacity);
        counterparty_id.reserve(capacity);
        bid.reserve(capacity);
        ask.reserve(capacity);
        volume.reserve(capacity);
    }
    
    void clear() {
        timestamp_ns.clear();
        instrument_id.clear();
        venue_id.clear();
        route_id.clear();
        counterparty_id.clear();
        bid.clear();
        ask.clear();
        volume.clear();
    }
};

class IngestionLoop {
public:
    IngestionLoop();
    ~IngestionLoop();

    void start(const std::string& nats_url);
    void stop();

    // Access to current buffers for the Lookback Bridge / SWMR Chunker
    std::shared_ptr<WarmSoABuffer> getActiveBuffer();
    void rotateBuffer();

private:
    void loop();
    void processMessage(natsMsg* msg);

    std::atomic<bool> isRunning_{false};
    std::thread loopThread_;

    natsConnection* nc_ = nullptr;
    jsCtx* js_ = nullptr;
    natsSubscription* sub_ = nullptr;

    std::shared_ptr<WarmSoABuffer> activeBuffer_;
    std::mutex bufferMutex_;
    
    static constexpr size_t BUFFER_CAPACITY = 100000;
};

} // namespace annex
} // namespace quanux
