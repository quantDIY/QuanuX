#pragma once
#include <functional>
#include <future>
#include <string>

namespace quanux::common {

class AIGateway {
public:
  virtual ~AIGateway() = default;

  /**
   * Synchronous query to the AI model.
   * Warning: This MAY BLOCK for a significant time.
   * Prefer using this in on_start or separate threads.
   */
  virtual std::string query(const std::string &prompt) = 0;

  /**
   * Asynchronous query. Returns a future to the result.
   */
  virtual std::future<std::string> query_async(const std::string &prompt) = 0;

  /**
   * Status check.
   */
  virtual bool is_connected() const = 0;
};

} // namespace quanux::common
