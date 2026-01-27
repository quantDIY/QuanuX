#include "native_ai_bridge.h"
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct NativeAIBridge::Impl {
  std::string base_url;
  std::string api_key;
  std::string model;
  std::unique_ptr<httplib::Client> client;

  Impl(const std::string &url, const std::string &key, const std::string &mdl)
      : base_url(url), api_key(key), model(mdl) {
    // Parse host and port from URL roughly or let httplib handle it
    // httplib::Client expects "http://host:port"
    client = std::make_unique<httplib::Client>(base_url.c_str());
    client->set_connection_timeout(5, 0); // 5s connection timeout
    client->set_read_timeout(10,
                             0); // 10s read timeout (fast inference required)
  }
};

NativeAIBridge::NativeAIBridge(const std::string &endpoint,
                               const std::string &api_key,
                               const std::string &model)
    : impl_(std::make_shared<Impl>(endpoint, api_key, model)) {}

NativeAIBridge::~NativeAIBridge() {}

std::string NativeAIBridge::query(const std::string &prompt) {
  json payload = {
      {"model", impl_->model},
      {"messages", {{{"role", "user"}, {"content", prompt}}}},
      {"max_tokens", 100}, // Limit response size for speed
      {"temperature", 0.0} // Deterministic
  };

  httplib::Headers headers = {{"Content-Type", "application/json"},
                              {"Authorization", "Bearer " + impl_->api_key}};

  auto res = impl_->client->Post("/v1/chat/completions", headers,
                                 payload.dump(), "application/json");

  if (res && res->status == 200) {
    try {
      auto response_json = json::parse(res->body);
      if (response_json.contains("choices") &&
          !response_json["choices"].empty()) {
        return response_json["choices"][0]["message"]["content"]
            .get<std::string>();
      }
    } catch (const std::exception &e) {
      std::cerr << "[AIBridge] JSON Parse Error: " << e.what() << std::endl;
      return "Error: Parse Failure";
    }
  } else {
    if (res) {
      std::cerr << "[AIBridge] HTTP Error: " << res->status
                << " Body: " << res->body << std::endl;
      return "Error: HTTP " + std::to_string(res->status);
    } else {
      std::cerr << "[AIBridge] Connection Failed: " << to_string(res.error())
                << std::endl;
      return "Error: Connection Failed";
    }
  }
  return "Error: Unknown";
}

std::future<std::string>
NativeAIBridge::query_async(const std::string &prompt) {
  // Simple std::async wrapper for now.
  // In production, this should use a thread pool or httplib's async features if
  // available (httplib is blocking sync usually).
  return std::async(std::launch::async,
                    [this, prompt]() { return this->query(prompt); });
}

bool NativeAIBridge::is_connected() const {
  // Simple health check
  auto res = impl_->client->Get("/v1/models");
  return (res && res->status == 200);
}
