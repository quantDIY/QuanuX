#include "MessagePublisher.hpp"
#include <iostream>

namespace QuanuX::ClearStreet {

MessagePublisher::MessagePublisher(const std::string &natsUrl)
    : conn_(nullptr), opts_(nullptr) {
  natsOptions_Create(&opts_);
  if (!natsUrl.empty()) {
    natsOptions_SetURL(opts_, natsUrl.c_str());
  }
}

MessagePublisher::~MessagePublisher() {
  if (conn_)
    natsConnection_Destroy(conn_);
  if (opts_)
    natsOptions_Destroy(opts_);
}

bool MessagePublisher::connect() {
  natsStatus s = natsConnection_Connect(&conn_, opts_);
  if (s == NATS_OK) {
    return true;
  } else {
    std::cerr << "NATS Connect Failed: " << natsStatus_GetText(s) << std::endl;
    return false;
  }
}

void MessagePublisher::publish(const std::string &subject,
                               const std::string &payload) {
  publish(subject, payload.c_str(), (int)payload.size());
}

void MessagePublisher::publish(const std::string &subject, const void *data,
                               int len) {
  if (!conn_)
    return;
  natsConnection_Publish(conn_, subject.c_str(), data, len);
}

} // namespace QuanuX::ClearStreet
