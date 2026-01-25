#pragma once

#include <concepts>
#include <ranges>
#include <type_traits>

namespace quanux::indicators {

/**
 * @brief Concept for numeric types supported by indicators
 * Must be arithmetic (int, float, double)
 */
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

/**
 * @brief Concept for a time-series input range
 * Must be a range of numeric values
 */
template <typename R>
concept TimeSeries =
    std::ranges::range<R> && Numeric<std::ranges::range_value_t<R>>;

/**
 * @brief Concept for storage policy (Auction Market Theory)
 */
template <typename P>
concept StoragePolicy = requires(P p, double price) {
  // Renaming to generic add/get for both TPO and Volume
  { p.add(price, 1.0) } -> std::same_as<void>; // Supports weighted add
  {
    p.get(price)
  } -> std::convertible_to<double>; // Return value can be TPO count or Volume
  { p.clear() } -> std::same_as<void>;
};

} // namespace quanux::indicators
