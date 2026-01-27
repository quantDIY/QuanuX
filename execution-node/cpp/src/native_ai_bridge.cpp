#include "native_ai_bridge.h"
#include <httplib.h>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

struct NativeAIBridge::Impl {
  std::string base_url;
  std::string api_key;
  std::string model;
  std::string provider;
  std::unique_ptr<httplib::Client> client;

  Impl(const std::string &url, const std::string &key, const std::string &mdl,
       const std::string &prov)
      : base_url(url), api_key(key), model(mdl), provider(prov) {

    if (base_url == "AUTODETECT" || base_url.empty()) {
      discover_endpoint();
    }

    std::cout << "[AIBridge] Connecting to " << base_url << " (" << provider
              << ")" << std::endl;
    client = std::make_unique<httplib::Client>(base_url.c_str());
    client->set_connection_timeout(5, 0); // 5s connection timeout
    client->set_read_timeout(30, 0);      // Extended for generic models
  }

  void discover_endpoint() {
    // Priority 1: Ollama (11434)
    if (check_port("http://localhost:11434")) {
      base_url = "http://localhost:11434";
      if (provider == "openai")
        provider =
            "ollama"; // Ollama supports openai API, so set provider to ollama
      return;
    }
    // Priority 2: Llama.cpp (8080)
    if (check_port("http://localhost:8080")) {
      base_url = "http://localhost:8080";
      return;
    }
    // Priority 3: LM Studio (1234)
    if (check_port("http://localhost:1234")) {
      base_url = "http://localhost:1234";
      return;
    }
    // Default fallback
    base_url = "http://localhost:8080";
    std::cerr << "[AIBridge] Warning: Auto-discovery failed, defaulting to "
              << base_url << std::endl;
  }

  bool check_port(const std::string &url) {
    try {
      auto cli = httplib::Client(url.c_str());
      cli.set_connection_timeout(1, 0);
      auto res = cli.Get("/v1/models"); // Standard probe
      return (res && res->status == 200);
    } catch (...) {
      return false;
    }
  }
};

NativeAIBridge::NativeAIBridge(const std::string &endpoint,
                               const std::string &api_key,
                               const std::string &model,
                               const std::string &provider)
    : impl_(std::make_shared<Impl>(endpoint, api_key, model, provider)) {}

NativeAIBridge::~NativeAIBridge() {}

std::string NativeAIBridge::query(const std::string &prompt) {
  // Common Payload Builder
  json payload;
  std::string endpoint = "/v1/chat/completions";

  if (impl_->provider == "openai" || impl_->provider == "ollama") {
    payload = {{"model", impl_->model},
               {"messages", {{{"role", "user"}, {"content", prompt}}}},
               {"max_tokens", 256},
               {"temperature", 0.7}};
  } else if (impl_->provider == "anthropic" || impl_->provider == "gemini") {
    // Placeholder for future formats (often proxied via OpenAI format locally
    // anyway) If native protocols required: endpoint = "/v1/messages"; //
    // Anthropic
    payload = {{"model", impl_->model},
               {"messages", {{{"role", "user"}, {"content", prompt}}}}};
  }

  httplib::Headers headers = {{"Content-Type", "application/json"},
                              {"Authorization", "Bearer " + impl_->api_key}};

  auto res = impl_->client->Post(endpoint.c_str(), headers, payload.dump(),
                                 "application/json");

  if (res && res->status == 200) {
    try {
      auto response_json = json::parse(res->body);

      // Provider-specific parsing
      if (impl_->provider == "openai" || impl_->provider == "ollama") {
        if (response_json.contains("choices") &&
            !response_json["choices"].empty()) {
          return response_json["choices"][0]["message"]["content"]
              .get<std::string>();
        }
      }
      // Fallback
      return res->body;
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
