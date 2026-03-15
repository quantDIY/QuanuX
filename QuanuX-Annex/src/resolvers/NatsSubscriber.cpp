#include "quanux/annex/do/NatsSubscriber.hpp"
#include <iostream>
#include <thread>
#include <chrono>
#include <stdexcept>
#include <cstring>
#include "nats.h"
#include "quanux/annex/TelemetryExhaust.hpp"

namespace quanux {
namespace annex {
namespace do_impl {

// Helper to cast opaque pointers
#define NC(ptr) static_cast<natsConnection*>(ptr)
#define JS(ptr) static_cast<jsCtx*>(ptr)

NatsSubscriber::NatsSubscriber(const std::string& nats_url, const std::string& stream_name)
    : m_nats_url(nats_url), m_stream_name(stream_name), m_running(false), m_connection(nullptr), m_jetstream(nullptr) {
    natsOptions* opts = nullptr;
    natsOptions_Create(&opts);
    natsOptions_SetURL(opts, m_nats_url.c_str());

    natsConnection* nc = nullptr;
    std::cout << "[NatsSubscriber] Firing Dynamic C-API connection to " << m_nats_url << "..." << std::endl;
    natsStatus s = natsConnection_Connect(&nc, opts);
    natsOptions_Destroy(opts);

    if (s != NATS_OK) {
        throw std::runtime_error(std::string("Failed to connect to NATS: ") + natsStatus_GetText(s));
    }
    m_connection = nc;

    jsOptions jsOpts;
    jsOptions_Init(&jsOpts);
    jsCtx* js = nullptr;
    
    s = natsConnection_JetStream(&js, nc, &jsOpts);
    if (s != NATS_OK) {
        throw std::runtime_error(std::string("Failed to initialize JetStream: ") + natsStatus_GetText(s));
    }
    m_jetstream = js;

    std::cout << "[NatsSubscriber] Connected zero-allocation C client to NATS JetStream at " << nats_url << " for stream: " << stream_name << "\n";
}

NatsSubscriber::~NatsSubscriber() {
    stop();
    if (m_jetstream) {
        jsCtx_Destroy(JS(m_jetstream));
    }
    if (m_connection) {
        natsConnection_Destroy(NC(m_connection));
    }
}

// Static handlers for C API callbacks
static void on_market_tick(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure) {
    auto* cb = static_cast<NatsSubscriber::MarketTickCallback*>(closure);
    if (cb && *cb) {
        if (natsMsg_GetDataLength(msg) == sizeof(MarketTick)) {
            MarketTick tick;
            std::memcpy(&tick, natsMsg_GetData(msg), sizeof(MarketTick));
            (*cb)(tick);
        } else {
            std::cerr << "[NatsSubscriber] Dropped payload: size mismatch for MarketTick\n";
        }
    }
    natsMsg_Destroy(msg);
}

static void on_execution_log(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure) {
    auto* cb = static_cast<NatsSubscriber::ExecutionLogCallback*>(closure);
    if (cb && *cb) {
        if (natsMsg_GetDataLength(msg) == sizeof(ExecutionLog)) {
            ExecutionLog exec;
            std::memcpy(&exec, natsMsg_GetData(msg), sizeof(ExecutionLog));
            (*cb)(exec);
        } else {
            std::cerr << "[NatsSubscriber] Dropped payload: size mismatch for ExecutionLog\n";
        }
    }
    natsMsg_Destroy(msg);
}

static void on_schema_drift(natsConnection *nc, natsSubscription *sub, natsMsg *msg, void *closure) {
    auto* cb = static_cast<NatsSubscriber::SchemaDriftCallback*>(closure);
    if (cb && *cb) {
        if (natsMsg_GetDataLength(msg) == sizeof(SchemaDriftEvent)) {
            SchemaDriftEvent drift;
            std::memcpy(&drift, natsMsg_GetData(msg), sizeof(SchemaDriftEvent));
            (*cb)(drift);
        } else {
            std::cerr << "[NatsSubscriber] Dropped payload: size mismatch for SchemaDriftEvent\n";
        }
    }
    natsMsg_Destroy(msg);
}

void NatsSubscriber::subscribe_market_data(const std::string& subject, MarketTickCallback on_tick) {
    auto* cb = new MarketTickCallback(on_tick); // Note: deliberately persisting for the life of the subscriber
    natsSubscription* sub = nullptr;
    natsStatus s = natsConnection_Subscribe(&sub, NC(m_connection), subject.c_str(), on_market_tick, cb);
    if (s == NATS_OK) {
        std::cout << "[NatsSubscriber] Active Pipeline: Subscribed to " << subject << "\n";
    } else {
        std::cerr << "[NatsSubscriber] Pipeline Failure: " << natsStatus_GetText(s) << "\n";
    }
}

void NatsSubscriber::subscribe_executions(const std::string& subject, ExecutionLogCallback on_exec) {
    auto* cb = new ExecutionLogCallback(on_exec);
    natsSubscription* sub = nullptr;
    natsStatus s = natsConnection_Subscribe(&sub, NC(m_connection), subject.c_str(), on_execution_log, cb);
    if (s == NATS_OK) {
        std::cout << "[NatsSubscriber] Active Pipeline: Subscribed to " << subject << "\n";
    } else {
        std::cerr << "[NatsSubscriber] Pipeline Failure: " << natsStatus_GetText(s) << "\n";
    }
}

void NatsSubscriber::subscribe_anomalies(const std::string& subject, SchemaDriftCallback on_drift) {
    auto* cb = new SchemaDriftCallback(on_drift);
    natsSubscription* sub = nullptr;
    natsStatus s = natsConnection_Subscribe(&sub, NC(m_connection), subject.c_str(), on_schema_drift, cb);
    if (s == NATS_OK) {
        std::cout << "[NatsSubscriber] Active Pipeline: Subscribed to " << subject << "\n";
    } else {
        std::cerr << "[NatsSubscriber] Pipeline Failure: " << natsStatus_GetText(s) << "\n";
    }
}

void NatsSubscriber::run_event_loop() {
    m_running = true;
    while(m_running) {
        // The nats.c callbacks are asynchronous and run on an internal thread pool.
        // The main thread simply waits to hold the loop open.
        std::this_thread::sleep_for(std::chrono::milliseconds(250));
    }
}

void NatsSubscriber::stop() {
    m_running = false;
}

} // namespace do_impl
} // namespace annex
} // namespace quanux
