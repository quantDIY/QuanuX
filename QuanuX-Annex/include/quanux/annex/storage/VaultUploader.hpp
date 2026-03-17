#pragma once
#include <string>
#include <deque>
#include <mutex>
#include <condition_variable>
#include <thread>
#include <atomic>

namespace quanux {
namespace annex {
namespace storage {

class VaultUploader {
public:
    VaultUploader();
    ~VaultUploader();

    void start();
    void stop();

    // Invoked by the HDF5Chunker when a SWMR file is safely sealed
    void queueArtifactForUpload(const std::string& filepath);

private:
    void workerLoop();
    bool uploadToGcs(const std::string& filepath);
    bool verifyChecksum(const std::string& filepath);

    std::atomic<bool> isRunning_{false};
    std::thread workerThread_;

    std::deque<std::string> uploadQueue_;
    std::mutex queueMutex_;
    std::condition_variable cv_;
};

} // namespace storage
} // namespace annex
} // namespace quanux
