#include "quanux/annex/IngestionLoop.hpp"
#include "quanux/annex/TranslationMatrix.hpp"
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
    const char* subj = natsMsg_GetSubject(msg);
    std::string subjectStr(subj);
    std::string venueCode = "UNKNOWN";
    
    size_t firstDot = subjectStr.find('.');
    size_t secondDot = subjectStr.find('.', firstDot + 1);
    
    if (firstDot != std::string::npos && secondDot != std::string::npos) {
        venueCode = subjectStr.substr(firstDot + 1, secondDot - firstDot - 1);
    }
    
    int32_t instrument_id = TranslationMatrix::getInstance().mapVenueToInstrumentId(venueCode);
    
    std::lock_guard<std::mutex> lock(bufferMutex_);
    if (activeBuffer_->timestamp_ns.size() >= BUFFER_CAPACITY) {
        rotateBuffer();
    }
    
    // Simulate appending the payload to the contiguous SoA
    activeBuffer_->timestamp_ns.push_back(1680000000000000000LL);
    activeBuffer_->instrument_id.push_back(instrument_id);
    activeBuffer_->bid.push_back(150.25f);
    activeBuffer_->ask.push_back(150.26f);
    activeBuffer_->volume.push_back(100);
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
