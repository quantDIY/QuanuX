#pragma once
#include <string>
#include <memory>

namespace quanux {
namespace annex {

class IngestionLoop; // Forward declaration

namespace lookback {

class LookbackIpcServer {
public:
    LookbackIpcServer(std::shared_ptr<IngestionLoop> ingestionLoop);
    ~LookbackIpcServer();

    // Start the Arrow IPC Memory-Mapped Socket listener
    // This provides the bi-directional bridge to the Aleph superGraph
    void start(const std::string& socketPath = "/tmp/quanux_lookback.arrow");
    void stop();

private:
    std::shared_ptr<IngestionLoop> loop_;
    bool isRunning_ = false;
};

} // namespace lookback
} // namespace annex
} // namespace quanux
