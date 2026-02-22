#pragma once
#include <iostream>
#include <nats.h>
#include <string>

namespace quanux::engine {

class NatsReplayer {
  natsConnection *conn = nullptr;
  natsOptions *opts = nullptr;
  bool enabled_ = false;

public:
  NatsReplayer(const std::string &url = "nats://localhost:4222");
  ~NatsReplayer();

  void publish_tick(const std::string &symbol, uint64_t ts, double price,
                    uint32_t size, bool is_bid);

  // Broadcast exhaustive metrics to the React Dashboard
  void publish_metrics(const std::string &strategy_id,
                       const std::string &json_payload);
};

} // namespace quanux::engine
