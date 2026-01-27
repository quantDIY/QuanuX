#pragma once
#include "quanux/common/AIGateway.h"
#include <memory>
#include <string>

class NativeAIBridge : public quanux::common::AIGateway {
public:
  // endpoint: e.g. "http://localhost:8080" or "AUTODETECT"
  // model: e.g. "llama-3-8b"
  // provider: "openai" (default), "ollama", "gemini_local"
  NativeAIBridge(const std::string &endpoint, const std::string &api_key,
                 const std::string &model,
                 const std::string &provider = "openai");
  ~NativeAIBridge() override;

  std::string query(const std::string &prompt) override;
  std::future<std::string> query_async(const std::string &prompt) override;
  bool is_connected() const override;

private:
  struct Impl;
  std::shared_ptr<Impl> impl_;
};
