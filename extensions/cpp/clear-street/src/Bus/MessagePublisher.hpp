#pragma once

#include <nats.h>
#include <string>
#include <vector>

namespace QuanuX::ClearStreet {

class MessagePublisher {
public:
  MessagePublisher(const std::string &natsUrl = "nats://localhost:4222");
  ~MessagePublisher();

  bool connect();

  // Publish raw bytes or JSON string
  void publish(const std::string &subject, const std::string &payload);
  void publish(const std::string &subject, const void *data, int len);

private:
  natsConnection *conn_;
  natsOptions *opts_;
};

} // namespace QuanuX::ClearStreet
