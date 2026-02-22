#include "engine/NatsReplayer.h"
#include <cstdio>

namespace quanux::engine {

NatsReplayer::NatsReplayer(const std::string &url) {
  if (natsOptions_Create(&opts) == NATS_OK) {
    natsOptions_SetURL(opts, url.c_str());
    if (natsConnection_Connect(&conn, opts) == NATS_OK) {
      enabled_ = true;
      std::cout << "[NatsReplayer] Connected to NATS at " << url << std::endl;
    } else {
      std::cerr << "[NatsReplayer] Failed to connect to NATS" << std::endl;
    }
  }
}

NatsReplayer::~NatsReplayer() {
  if (conn)
    natsConnection_Destroy(conn);
  if (opts)
    natsOptions_Destroy(opts);
}

void NatsReplayer::publish_tick(const std::string &symbol, uint64_t ts,
                                double price, uint32_t size, bool is_bid) {
  if (!enabled_)
    return;

  // Simple JSON format for now: {"T": "t", "S": "ES", "p": 5000.0, "s": 1, "b":
  // true, "ts": 123456789}
  char buffer[256];
  snprintf(buffer, sizeof(buffer),
           "{\"T\":\"t\", \"S\":\"%s\", \"p\":%.2f, \"s\":%u, \"b\":%s, "
           "\"ts\":%llu}",
           symbol.c_str(), price, size, is_bid ? "true" : "false", ts);

  std::string subject = "market.data." + symbol;
  natsConnection_PublishString(conn, subject.c_str(), buffer);
}

void NatsReplayer::publish_metrics(const std::string &strategy_id,
                                   const std::string &json_payload) {
  if (!enabled_)
    return;

  std::string subject = "sys.crucible.report." + strategy_id;
  natsConnection_PublishString(conn, subject.c_str(), json_payload.c_str());
}

} // namespace quanux::engine
