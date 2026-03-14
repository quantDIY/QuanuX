#pragma once

#include "quanux/annex/IStorageResolver.hpp"
#include "quanux/annex/do/NatsSubscriber.hpp"
#include "quanux/annex/do/ZarrRamFlusher.hpp"
#include <memory>
#include <string>

namespace quanux {
namespace annex {

/**
 * @brief DigitalOcean Spaces Virtual Filesystem Resolver.
 * Implements IStorageResolver using Zarr chunks over S3-compatible object storage.
 */
class ZarrResolver_DO : public IStorageResolver {
public:
    ZarrResolver_DO(const std::string& space_name, const std::string& space_region);
    virtual ~ZarrResolver_DO();

    // IStorageResolver Implementation
    void broadcast_live_markdown(const std::string& markdown) override;
    std::string get_historical_analytics(const std::string& query) override;

    // Initializes the internal sub-components (Subscriber & Flusher)
    void start_ingestion();
    void stop_ingestion();

private:
    std::string m_space_name;
    std::string m_space_region;
    
    std::unique_ptr<do_impl::NatsSubscriber> m_subscriber;
    std::unique_ptr<do_impl::ZarrRamFlusher> m_flusher;
};

} // namespace annex
} // namespace quanux
