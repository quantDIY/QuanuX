#pragma once

#include "quanux/annex/IStorageResolver.hpp"
#include <memory>
#include <string>

namespace quanux {
namespace annex {
namespace federation {

/**
 * @brief The C++ Microservice Entry Point for Hasura Federation.
 * 
 * Exposes a headless GraphQL HTTP/WebSocket endpoint that strictly acts as a
 * Remote Schema for the master Aleph Protocol Hasura API Gateway.
 *
 * It enforces the 4 Unalterable Mandates:
 * 1. Execution Exhaust (Live Routing)
 * 2. Live Market Data (Live Routing)
 * 3. Cluster Monitoring (Live State)
 * 4. Deep Storage Retrieval (Historical via IStorageResolver - NO MATH)
 */
class HasuraRemoteWorker {
public:
    /**
     * @brief Constructor injects the cloud-agnostic IStorageResolver.
     * @param resolver The configured storage resolver (DO, AWS, GCP, AZURE)
     * @param port The HTTP/WS port to bind the remote schema to (e.g., 8080)
     */
    HasuraRemoteWorker(std::shared_ptr<IStorageResolver> resolver, uint16_t port);
    ~HasuraRemoteWorker();

    // Bootstraps the embedded HTTP/WebSocket server for Hasura to federate with
    void start_server();
    void stop_server();

private:
    std::shared_ptr<IStorageResolver> m_resolver;
    uint16_t m_port;

    // Internal mechanics to bind GraphQL routes and NATS subscriptions
    void bind_query_routes();
    void bind_subscription_routes();
};

} // namespace federation
} // namespace annex
} // namespace quanux
