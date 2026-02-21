#pragma once

#include "../IEngine.hpp"
#include <memory>
#include <string>

namespace QuanuX::ClearStreet {

// Abstract Strategy Interface (must be implemented by shared lib)
class IStrategy {
public:
  virtual ~IStrategy() = default;
  virtual void onMarketData(const std::string &symbol, double bid,
                            double ask) = 0;
  virtual void onExecution(const ExecutionReport &report) = 0;
  virtual void setEngine(IEngine *engine) {}
};

class StrategyLoader {
public:
  StrategyLoader(const std::string &libPath);
  ~StrategyLoader();

  bool load();
  bool reload();

  IStrategy *getStrategy() const;

private:
  std::string libPath_;
  void *handle_;
  IStrategy *instance_;

  // Function pointer types
  using CreateStrategyFn = IStrategy *(*)();
  using DestroyStrategyFn = void (*)(IStrategy *);

  CreateStrategyFn createFn_;
  DestroyStrategyFn destroyFn_;

  void unload();
};

} // namespace QuanuX::ClearStreet
