#include "nats_bridge.h"
#include <iostream>
#include <string>

NatsBridge::NatsBridge(const std::string &url) {
  natsStatus s = natsOptions_Create(&opts);
  if (s == NATS_OK)
    s = natsOptions_SetURL(opts, url.c_str());
  if (s == NATS_OK)
    s = natsConnection_Connect(&conn, opts);

  if (s == NATS_OK) {
    std::cout << "[NatsBridge] Connected to " << url << std::endl;
  } else {
    std::cerr << "[NatsBridge] Failed to connect: " << natsStatus_GetText(s)
              << std::endl;
  }
}

NatsBridge::~NatsBridge() {
  if (conn)
    natsConnection_Destroy(conn);
  if (opts)
    natsOptions_Destroy(opts);
}

void NatsBridge::publish(const std::string &subject, const std::string &data) {
  if (!conn)
    return;
  natsStatus s =
      natsConnection_PublishString(conn, subject.c_str(), data.c_str());
  if (s != NATS_OK) {
    std::cerr << "[NatsBridge] Publish failed: " << natsStatus_GetText(s)
              << std::endl;
  }
}

void NatsBridge::publish_trade(uint64_t order_id, double price,
                               double quantity) {
  std::string json = "{ \"order_id\": " + std::to_string(order_id) +
                     ", \"price\": " + std::to_string(price) +
                     ", \"quantity\": " + std::to_string(quantity) + " }";
  publish("node.telemetry.fills", json);
}

void NatsBridge::publish_market_data(const std::string &symbol, double price,
                                     double size, bool is_trade) {
  std::string topic = "market.data." + symbol;
  std::string json = "{ \"symbol\": \"" + symbol + "\", " +
                     "\"price\": " + std::to_string(price) + ", " +
                     "\"size\": " + std::to_string(size) + ", " +
                     "\"type\": \"" + (is_trade ? "trade" : "quote") + "\" }";
  publish(topic, json);
}
