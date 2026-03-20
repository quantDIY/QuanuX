#include "quanux/annex/IngestionLoop.hpp"
#include "quanux/annex/TranslationMatrix.hpp"
#include "quanux/annex/TelemetryExhaust.hpp"
#include <iostream>
#include <cstring>

namespace quanux {
namespace annex {

IngestionLoop::IngestionLoop() {
    activeBuffer_ = std::make_shared<WarmSoABuffer>();
    activeBuffer_->reserve(BUFFER_CAPACITY);
}

IngestionLoop::~IngestionLoop() {
    stop();
}

void IngestionLoop::start(const std::string& nats_url) {
    if (isRunning_) return;
    
    natsStatus s = natsConnection_ConnectTo(&nc_, nats_url.c_str());
    if (s != NATS_OK) {
        std::cerr << "[FATAL] IngestionLoop Failed to connect to NATS: " << natsStatus_GetText(s) << std::endl;
        return;
    }
    
    jsOptions jsOpts;
    jsOptions_Init(&jsOpts);
    s = natsConnection_JetStream(&js_, nc_, &jsOpts);
    if (s != NATS_OK) {
        std::cerr << "[FATAL] IngestionLoop Failed to create JetStream Context: " << natsStatus_GetText(s) << std::endl;
        return;
    }
    
    // Bind to the durable pull consumer ANNEX_PROCESSOR on QUANUX_INGEST
    jsSubOptions subOpts;
    jsSubOptions_Init(&subOpts);
    subOpts.Stream = "QUANUX_INGEST";
    subOpts.Consumer = "ANNEX_PROCESSOR";
    
    jsErrCode errCode;
    s = js_PullSubscribe(&sub_, js_, "VENUE.*.RAW.>", "ANNEX_PROCESSOR", &jsOpts, &subOpts, &errCode);
    if (s != NATS_OK) {
        std::cerr << "[FATAL] IngestionLoop Failed to subscribe to JetStream: " << natsStatus_GetText(s) << std::endl;
        return;
    }

    isRunning_ = true;
    loopThread_ = std::thread(&IngestionLoop::loop, this);
    std::cout << "[ANNEX] IngestionLoop Started. Listening on JetStream ANNEX_PROCESSOR." << std::endl;
}

void IngestionLoop::stop() {
    if (!isRunning_) return;
    isRunning_ = false;
    if (loopThread_.joinable()) loopThread_.join();
    
    if (sub_) natsSubscription_Destroy(sub_);
    if (js_) jsCtx_Destroy(js_);
    if (nc_) natsConnection_Destroy(nc_);
    
    std::cout << "[ANNEX] IngestionLoop Stopped." << std::endl;
}

void IngestionLoop::loop() {
    while (isRunning_) {
        natsMsgList msgList;
        // Fetch up to 100 messages or wait up to 1000ms
        natsStatus s = natsSubscription_Fetch(&msgList, sub_, 100, 1000, nullptr);
        
        if (s == NATS_OK) {
            for (int i = 0; i < msgList.Count; ++i) {
                natsMsg* msg = msgList.Msgs[i];
                processMessage(msg);
                // Explicit ACK required by architecture to safeguard Sovereign Vault data
                natsMsg_Ack(msg, nullptr);
                natsMsg_Destroy(msg);
            }
            natsMsgList_Destroy(&msgList);
        }
    }
}

void IngestionLoop::processMessage(natsMsg* msg) {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    if (activeBuffer_->timestamp_ns.size() >= BUFFER_CAPACITY) {
        rotateBuffer();
    }
    
    // Validate Structural Boundaries Natively
    if (natsMsg_GetDataLength(msg) == sizeof(MarketTick)) {
        MarketTick* tick = reinterpret_cast<MarketTick*>(const_cast<char*>(natsMsg_GetData(msg)));
        
        auto& tMatrix = TranslationMatrix::getInstance();
        if (!tMatrix.isValidIdentitySet(tick->venue_id, tick->route_id, tick->counterparty_id)) {
            std::cerr << "[ANNEX] Dropped payload: Route/Venue capabilities mismatch or contradictory combinations. Identity validation failed natively." << std::endl;
            return;
        }

        activeBuffer_->timestamp_ns.push_back(tick->timestamp_ns);
        activeBuffer_->instrument_id.push_back(tick->instrument_id);
        activeBuffer_->venue_id.push_back(tick->venue_id);
        activeBuffer_->route_id.push_back(tick->route_id);
        activeBuffer_->counterparty_id.push_back(tick->counterparty_id);
        activeBuffer_->bid.push_back(static_cast<float>(tick->bid_price));
        activeBuffer_->ask.push_back(static_cast<float>(tick->ask_price));
        activeBuffer_->volume.push_back(tick->bid_size);
    } else {
        std::cerr << "[ANNEX] Dropped payload: structural schema mismatch constraints violated. Expected bytes: " 
                  << sizeof(MarketTick) << ", received: " << natsMsg_GetDataLength(msg) << std::endl;
    }
}

std::shared_ptr<WarmSoABuffer> IngestionLoop::getActiveBuffer() {
    std::lock_guard<std::mutex> lock(bufferMutex_);
    return activeBuffer_;
}

void IngestionLoop::rotateBuffer() {
    activeBuffer_ = std::make_shared<WarmSoABuffer>();
    activeBuffer_->reserve(BUFFER_CAPACITY);
}

} // namespace annex
} // namespace quanux
