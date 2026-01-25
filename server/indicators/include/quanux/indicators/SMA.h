#pragma once

#include "Indicator.h"
#include <deque>

namespace quanux::indicators {

class SMA : public IndicatorBase {
private:
  int period_;
  std::deque<double> window_;
  double sum_ = 0.0;

public:
  SMA(int period) : period_(period) {}

  double update(double value) {
    sum_ += value;
    window_.push_back(value);

    if (window_.size() > period_) {
      sum_ -= window_.front();
      window_.pop_front();
    }

    return this->value();
  }

  double value() const override {
    if (window_.empty())
      return 0.0;
    return sum_ / window_.size();
  }
};

} // namespace quanux::indicators
