#pragma once
#include <nats.h>
#include <string>

class NatsBridge {
  natsConnection *conn = nullptr;
  natsOptions *opts = nullptr;

public:
  explicit NatsBridge(const std::string &url);
  ~NatsBridge();

  void publish(const std::string &subject, const std::string &data);
  void publish_trade(uint64_t order_id, double price, double quantity);
  void publish_market_data(const std::string &symbol, double price, double size,
                           bool is_trade);
};
