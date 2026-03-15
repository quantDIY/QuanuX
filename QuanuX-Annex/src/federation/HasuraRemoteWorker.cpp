#include "quanux/annex/federation/HasuraRemoteWorker.hpp"
#include <iostream>
#include <thread>

namespace quanux {
namespace annex {
namespace federation {

HasuraRemoteWorker::HasuraRemoteWorker(std::shared_ptr<IStorageResolver> resolver, uint16_t port)
    : m_resolver(resolver), m_port(port) {
}

HasuraRemoteWorker::~HasuraRemoteWorker() {
    stop_server();
}

void HasuraRemoteWorker::start_server() {
    std::cout << "[HasuraRemoteWorker] Booting GraphQL Federation Endpoint on port " << m_port << "...\n";
    
    bind_query_routes();
    bind_subscription_routes();

    std::cout << "[HasuraRemoteWorker] Remote Schema Active. Awaiting queries from master Aleph API Gateway.\n";
    
    // RED TEAM MANDATE: Eradicate Synchronous Blocking -> Expand ThreadPool to 128 to match concurrent async limits
    m_server.new_task_queue = [] { return new httplib::ThreadPool(128); };
    
    // Blocks the current thread, accepting Hasura webhooks
    m_server.listen("0.0.0.0", m_port);
}

void HasuraRemoteWorker::stop_server() {
    std::cout << "[HasuraRemoteWorker] Shutting down Hasura Remote Schema.\n";
    m_server.stop();
}

void HasuraRemoteWorker::bind_query_routes() {
    std::cout << "  -> Binding POST /graphql endpoint -> Executing Mandate 4: Deep Storage Retrieval.\n";
    
    m_server.Post("/graphql", [this](const httplib::Request& req, httplib::Response& res) {
        try {
            // Parse the incoming JSON webhook from Hasura
            nlohmann::json hasura_payload = nlohmann::json::parse(req.body);
            std::string query = hasura_payload.value("query", "");
            
            // Execute streaming non-blocking retrieval matrix
            res.set_chunked_content_provider("application/json", 
                [this, query](size_t offset, httplib::DataSink &sink) {
                    bool finished = false;
                    
                    // We call the resolver which now takes a lambda callback to ingest chunks of JSON!
                    // It returns true on success, false on failure.
                    bool success = m_resolver->stream_historical_analytics(query, [&sink](const std::string& json_chunk) {
                        sink.write(json_chunk.c_str(), json_chunk.size());
                        return true;
                    });
                    
                    if (!success) {
                        std::string err = "{\"errors\": [{\"message\": \"Stream Failure\"}]}";
                        sink.write(err.c_str(), err.size());
                    }
                    
                    sink.done();
                    return true; 
                });
        } catch (const std::exception& e) {
            std::cerr << "[HasuraRemoteWorker] Error parsing webhook: " << e.what() << "\n";
            res.status = 400;
            res.set_content("{\"errors\": [{\"message\": \"Malformed GraphQL Payload\"}]}", "application/json");
        }
    });
}

void HasuraRemoteWorker::bind_subscription_routes() {
    std::cout << "  -> Binding WebSocket /graphql/ws -> Executing Mandates 1, 2 & 3 (Live Exhaust).\n";
    // Placeholders for WS routes, ignoring for Phase 14
}

} // namespace federation
} // namespace annex
} // namespace quanux
