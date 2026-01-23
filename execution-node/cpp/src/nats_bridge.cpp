#include <iostream>
#include <nats/nats.h>
#include <string>
#include <thread>
#include <vector>

class NatsBridge {
  natsConnection *conn = nullptr;
  natsOptions *opts = nullptr;

public:
  NatsBridge(const std::string &url) {
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

  ~NatsBridge() {
    if (conn)
      natsConnection_Destroy(conn);
    if (opts)
      natsOptions_Destroy(opts);
  }

  void publish(const std::string &subject, const std::string &data) {
    if (!conn)
      return;

    // Asynchronous publish via NATS C client (high performance)
    natsStatus s =
        natsConnection_PublishString(conn, subject.c_str(), data.c_str());
    if (s != NATS_OK) {
      std::cerr << "[NatsBridge] Publish failed: " << natsStatus_GetText(s)
                << std::endl;
    }
  }

  // Publish Trade Data (Reverse Pipe)
  void publish_trade(uint64_t order_id, double price, double quantity) {
    std::string json = "{ \"order_id\": " + std::to_string(order_id) +
                       ", \"price\": " + std::to_string(price) +
                       ", \"quantity\": " + std::to_string(quantity) + " }";
    publish("node.telemetry.fills", json);
  }
};
