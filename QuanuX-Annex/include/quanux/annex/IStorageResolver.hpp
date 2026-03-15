#pragma once

#include <string>
#include <functional>

namespace quanux {
namespace annex {

/**
 * @brief Pure virtual C++ interface for the Storage Abstraction Layer (SAL).
 * This ensures the core superGraph remains blind to underlying cloud physics.
 */
class IStorageResolver {
public:
    virtual ~IStorageResolver() = default;

    /**
     * @brief Live Broadcaster hook
     * @param markdown Payload dynamically synthesized into Markdown for AI agents.
     */
    virtual void broadcast_live_markdown(const std::string& markdown) = 0;

    /**
     * @brief Historical Query for pulling deep metrics
     * @param query Historical query strings.
     * @return Retrieved insights in robust Markdown formatting.
     */
    virtual std::string get_historical_analytics(const std::string& query) = 0;

    /**
     * @brief OOM-Guarded streaming JSON retrieval matrix. Pushes raw strings into the cpp-httplib HTTP chunked pipe.
     */
    virtual bool stream_historical_analytics(const std::string& query, std::function<bool(const std::string& chunk)> sink_cb) {
        sink_cb(get_historical_analytics(query));
        return true;
    }
};

} // namespace annex
} // namespace quanux
