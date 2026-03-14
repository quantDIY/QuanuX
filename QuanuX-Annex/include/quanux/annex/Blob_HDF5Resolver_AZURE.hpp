#pragma once

#include "quanux/annex/IStorageResolver.hpp"
#include <string>

namespace quanux {
namespace annex {

/**
 * @brief Azure ADLS Gen2 Virtual File System Resolver.
 * Uses the HDF5 REST Virtual File Driver (VFD) concept to fetch specific byte-ranges
 * via HTTP GET requests directly from Azure Blob Storage.
 * Operates purely on the Read Path.
 */
class Blob_HDF5Resolver_AZURE : public IStorageResolver {
public:
    Blob_HDF5Resolver_AZURE(const std::string& account_name, const std::string& container_name);
    virtual ~Blob_HDF5Resolver_AZURE();

    // IStorageResolver Implementation
    void broadcast_live_markdown(const std::string& markdown) override;
    std::string get_historical_analytics(const std::string& query) override;

private:
    std::string m_account_name;
    std::string m_container_name;
};

} // namespace annex
} // namespace quanux
