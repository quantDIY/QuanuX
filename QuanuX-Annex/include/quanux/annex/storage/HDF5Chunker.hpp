#pragma once
#include "quanux/annex/IngestionLoop.hpp"
#include <memory>
#include <string>
#include <thread>
#include <atomic>

namespace quanux {
namespace annex {
namespace storage {

// Runs on an isolated background I/O thread, watching the SoA ring buffers
// Converts Warm memory to immutable HDF5 chunks via c-blosc2
class HDF5Chunker {
public:
    HDF5Chunker(std::shared_ptr<IngestionLoop> loop);
    ~HDF5Chunker();

    void start();
    void stop();

private:
    void threadLoop();
    void appendToSWMR(std::shared_ptr<WarmSoABuffer> buffer);
    void checkSealingBoundary();

    std::shared_ptr<IngestionLoop> loop_;
    std::atomic<bool> isRunning_{false};
    std::thread chunkerThread_;
    
    std::string currentHdf5File_;
    size_t currentFileSize_ = 0;
};

} // namespace storage
} // namespace annex
} // namespace quanux
