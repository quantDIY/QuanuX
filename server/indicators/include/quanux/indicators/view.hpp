#pragma once

#include "quanux/indicators/concepts.hpp"
#include <iterator>
#include <ranges>

namespace quanux::indicators {

/**
 * @brief Base class for all indicator views.
 * Inherits from std::ranges::view_interface to provide begin/end/empty/etc
 * automatically.
 */
template <typename Derived, typename R>
  requires std::ranges::view<R>
class indicator_view : public std::ranges::view_interface<Derived> {
protected:
  R base_;

public:
  indicator_view() = default;
  constexpr explicit indicator_view(R base) : base_(std::move(base)) {}

  constexpr R const &base() const & { return base_; }
  constexpr R base() && { return std::move(base_); }
};

} // namespace quanux::indicators
