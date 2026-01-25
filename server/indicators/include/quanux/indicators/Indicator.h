#pragma once

#include <concepts>
#include <iostream>
#include <vector>

namespace quanux::indicators {

template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// Concept for any Indicator that processes a stream of type T
template <typename I, typename T>
concept IsIndicator = requires(I indicator, T value) {
  { indicator.update(value) } -> std::convertible_to<double>;
  { indicator.value() } -> std::convertible_to<double>;
};

// Base class for stateful indicators
class IndicatorBase {
public:
  virtual ~IndicatorBase() = default;
  virtual double value() const = 0;
};

} // namespace quanux::indicators
