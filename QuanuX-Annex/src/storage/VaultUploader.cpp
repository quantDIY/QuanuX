#include "quanux/annex/storage/VaultUploader.hpp"
#include <iostream>
#include <chrono>
#include <fstream>
#include <cstdio>
#include <google/cloud/storage/client.h>
#include <google/cloud/storage/well_known_headers.h>

namespace quanux {
namespace annex {
namespace storage {

VaultUploader::VaultUploader() {}

VaultUploader::~VaultUploader() { stop(); }

void VaultUploader::start() {
    if (isRunning_) return;
    isRunning_ = true;
    workerThread_ = std::thread(&VaultUploader::workerLoop, this);
    std::cout << "[SOVEREIGN_VAULT] GCP Async C++ Worker initialized. Listening for sealed HDF5 chunks." << std::endl;
}

void VaultUploader::stop() {
    if (!isRunning_) return;
    isRunning_ = false;
    cv_.notify_all();
    if (workerThread_.joinable()) workerThread_.join();
}

void VaultUploader::queueArtifactForUpload(const std::string& filepath) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        uploadQueue_.push_back(filepath);
    }
    cv_.notify_one();
}

void VaultUploader::workerLoop() {
    namespace gcs = google::cloud::storage;
    auto client = gcs::Client::CreateDefaultClient();
    if (!client) {
        std::cerr << "[SOVEREIGN_VAULT] Failed to initialize GCP Storage Client." << std::endl;
        return;
    }

    while (isRunning_) {
        std::string targetFile;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            cv_.wait(lock, [this]{ return !uploadQueue_.empty() || !isRunning_; });
            if (!isRunning_ && uploadQueue_.empty()) break;

            targetFile = uploadQueue_.front();
            uploadQueue_.pop_front();
        }

        std::string object_name = targetFile.substr(targetFile.find_last_of("/\\") + 1);
        std::cout << "[SOVEREIGN_VAULT] Executing google_cloud_cpp::storage Upload for: " << targetFile << std::endl;
        
        auto local_crc32c_status = gcs::ComputeCrc32cChecksum(targetFile);
        if (!local_crc32c_status) {
            std::cerr << "[SOVEREIGN_VAULT] Could not compute local CRC32c for " << targetFile << std::endl;
            continue;
        }
        std::string local_crc32c = *local_crc32c_status;

        auto metadata = client->UploadFile(targetFile, "google_storage_bucket", object_name);
        
        if (!metadata) {
            std::cerr << "[SOVEREIGN_VAULT] Failed to upload artifact: " << metadata.status().message() << std::endl;
            continue;
        }

        if (metadata->crc32c() == local_crc32c) {
            std::cout << "[SOVEREIGN_VAULT] Checksum Matched! Artifact replicated cleanly. Deleting NVMe cache: " << targetFile << std::endl;
            std::remove(targetFile.c_str());
        } else {
            std::cerr << "[SOVEREIGN_VAULT] CRITICAL: Checksum mismatch. GCP: " << metadata->crc32c() << " vs Local: " << local_crc32c << std::endl;
        }
    }
}

bool VaultUploader::uploadToGcs(const std::string& filepath) {
    return true; 
}

bool VaultUploader::verifyChecksum(const std::string& filepath) {
    return true;
}

} // namespace storage
} // namespace annex
} // namespace quanux
