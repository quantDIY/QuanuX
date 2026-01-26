#pragma once
#include "strategy_interface.h"
#include <functional>
#include <string>

// Abstract Feed Interface
class Feed {
public:
  virtual ~Feed() = default;

  // Connect to source
  virtual void connect() = 0;

  // Start streaming. Calls callback for every tick.
  // Blocking call (runs the event loop).
  virtual void start(std::function<void(const MarketUpdate &)> on_tick) = 0;

  // Stop streaming
  virtual void stop() = 0;
};
