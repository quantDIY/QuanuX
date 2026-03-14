#pragma once

#include "quanux/annex/IStorageResolver.hpp"
#include <string>

namespace quanux {
namespace annex {

/**
 * @brief AWS FSx File System Resolver.
 * Uses native POSIX filesystem I/O (via libhdf5) on the locally mounted S3 bucket.
 * Operates purely on the Read Path, completely decoupled from Ingestion.
 */
class FSxResolver_AWS : public IStorageResolver {
public:
    explicit FSxResolver_AWS(const std::string& mount_path);
    virtual ~FSxResolver_AWS();

    // IStorageResolver Implementation
    void broadcast_live_markdown(const std::string& markdown) override;
    std::string get_historical_analytics(const std::string& query) override;

private:
    std::string m_mount_path;
};

} // namespace annex
} // namespace quanux
