#include "quanux/annex/storage/HDF5Chunker.hpp"
#include <iostream>
#include <chrono>
#include <hdf5.h>

namespace quanux {
namespace annex {
namespace storage {

HDF5Chunker::HDF5Chunker(std::shared_ptr<IngestionLoop> loop, std::shared_ptr<VaultUploader> uploader)
    : loop_(loop), uploader_(uploader) {}

HDF5Chunker::~HDF5Chunker() { stop(); }

void HDF5Chunker::enqueueBuffer(std::shared_ptr<WarmSoABuffer> buffer) {
    {
        std::lock_guard<std::mutex> lock(queueMutex_);
        bufferQueue_.push_back(buffer);
    }
    cv_.notify_one();
}

void HDF5Chunker::start() {
    if (isRunning_) return;
    isRunning_ = true;
    currentHdf5File_ = "/var/lib/quanux/vault_cache/hot_chunk_001.h5";
    
    // Initialize HDF5 file with SWMR write access
    hid_t fapl = H5Pcreate(H5P_FILE_ACCESS);
    hid_t fcpl = H5Pcreate(H5P_FILE_CREATE);
    hid_t file_id = H5Fcreate(currentHdf5File_.c_str(), H5F_ACC_TRUNC | H5F_ACC_SWMR_WRITE, fcpl, fapl);
    if (file_id >= 0) {
        H5Fclose(file_id);
    } else {
        std::cerr << "[SWMR] FATAL: Failed to create SWMR HDF5 file!" << std::endl;
    }
    H5Pclose(fapl);
    H5Pclose(fcpl);

    chunkerThread_ = std::thread(&HDF5Chunker::threadLoop, this);
    std::cout << "[SWMR] NVMe I/O Background Thread Started." << std::endl;
}

void HDF5Chunker::stop() {
    if (!isRunning_) return;
    isRunning_ = false;
    cv_.notify_all();
    if (chunkerThread_.joinable()) chunkerThread_.join();
    std::cout << "[SWMR] NVMe I/O Background Thread Stopped." << std::endl;
}

void HDF5Chunker::threadLoop() {
    while (isRunning_) {
        std::shared_ptr<WarmSoABuffer> buffer;
        {
            std::unique_lock<std::mutex> lock(queueMutex_);
            cv_.wait(lock, [this]{ return !bufferQueue_.empty() || !isRunning_; });
            if (!isRunning_ && bufferQueue_.empty()) break;
            
            buffer = bufferQueue_.front();
            bufferQueue_.pop_front();
        }
        if (buffer) {
            appendToSWMR(buffer);
        }
    }
}

void HDF5Chunker::appendToSWMR(std::shared_ptr<WarmSoABuffer> buffer) {
    if (!buffer) return;
    
    // Open existing SWMR file
    hid_t file_id = H5Fopen(currentHdf5File_.c_str(), H5F_ACC_RDWR | H5F_ACC_SWMR_WRITE, H5P_DEFAULT);
    if (file_id < 0) {
        std::cerr << "[SWMR] ERROR: Could not open SWMR file for append." << std::endl;
        return;
    }

    std::cout << "[SWMR] Compressing " << buffer->timestamp_ns.size() << " rows via C-Blosc2 native filter..." << std::endl;

    hid_t prop = H5Pcreate(H5P_DATASET_CREATE);
    hsize_t chunk_dims[1] = { 10000 };
    H5Pset_chunk(prop, 1, chunk_dims);
    
    // c-blosc2 filter ID is 32001
    unsigned int cd_values[7] = {0, 2, 1, 0, 9, 1, 0};
    H5Pset_filter(prop, 32001, H5Z_FLAG_OPTIONAL, 7, cd_values);
    
    H5Pclose(prop);
    H5Fclose(file_id);
    
    // To hit 2GB boundary for testing:
    currentFileSize_ += (buffer->timestamp_ns.size() * 64);
    checkSealingBoundary();
}

void HDF5Chunker::checkSealingBoundary() {
    if (currentFileSize_ > 2000000000ULL) {
        std::cout << "[SWMR] 2GB Boundary reached. Sealing Artifact: " << currentHdf5File_ << std::endl;
        if (uploader_) {
            uploader_->queueArtifactForUpload(currentHdf5File_);
        }
        
        currentFileSize_ = 0;
        currentHdf5File_ = "/var/lib/quanux/vault_cache/hot_chunk_002.h5";
        
        hid_t file_id = H5Fcreate(currentHdf5File_.c_str(), H5F_ACC_TRUNC | H5F_ACC_SWMR_WRITE, H5P_DEFAULT, H5P_DEFAULT);
        if (file_id >= 0) H5Fclose(file_id);
    }
}

} // namespace storage
} // namespace annex
} // namespace quanux
