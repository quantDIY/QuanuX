#pragma once

#include <cstdint>
#include <string>

namespace QuanuX {

enum class Side { Buy, Sell, Short, Cover };

struct Order {
  int64_t id;
  std::string symbol;
  Side side;
  double quantity;
  double price;
  std::string currency = "USD";
  std::string exchange = "SMART";
};

class IExecutionProvider {
public:
  virtual ~IExecutionProvider() = default;
  virtual void sendOrder(const Order &order) = 0;
  virtual bool connect() = 0;
};

} // namespace QuanuX
