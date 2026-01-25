#pragma once

#include "quanux/indicators/concepts.hpp"
#include "quanux/indicators/view.hpp"
#include <cmath>
#include <limits>
#include <numeric>

namespace quanux::indicators {

// -------------------------------------------------------------------------
// SMA View Implementation
// -------------------------------------------------------------------------
template <std::ranges::view R>
  requires Numeric<std::ranges::range_value_t<R>>
class sma_view : public indicator_view<sma_view<R>, R> {
private:
  int period_;

public:
  sma_view() = default;
  constexpr sma_view(R base, int period)
      : indicator_view<sma_view<R>, R>(std::move(base)), period_(period) {}

  // Iterator
  class iterator {
  public:
    using iterator_category =
        std::forward_iterator_tag; // Upgrade to random_access later
    using value_type = double;
    using difference_type = std::ptrdiff_t;
    using pointer = const double *;
    using reference = double;

  private:
    std::ranges::iterator_t<R> current_;
    std::ranges::iterator_t<R> begin_;
    int period_;

  public:
    iterator() = default;
    constexpr iterator(std::ranges::iterator_t<R> current,
                       std::ranges::iterator_t<R> begin, int period)
        : current_(current), begin_(begin), period_(period) {}

    value_type operator*() const {
      // Ensure we have enough history
      if (std::distance(begin_, current_) < period_ - 1) {
        return std::numeric_limits<double>::quiet_NaN();
      }

      // Naive implementation O(N) per step - will optimize later with
      // caching/sliding window Note: Standard range algorithms are safer here
      double sum = 0.0;

      // Walk backwards logic
      // std::ranges doesn't guarantee backward traversal for forward iterators
      // So we actually need to implementation a sliding window or cache.
      // For this MVP, we re-scan from (current - period + 1) to current.

      // Optimized sliding window approach requires stateful iterator (caching
      // sum) For now, let's just do the safe lookback if iterator is
      // bidirectional If forward only, we are in trouble without caching.
      // Assumption: TimeSeries usually provides Random Access or Bidirectional
      // (vector/deque)

      // Let's assume Random Access for MVP simplicity on vector<double>
      auto start_it = current_ - (period_ - 1);
      for (int i = 0; i < period_; ++i) {
        sum += *(start_it + i);
      }

      return sum / period_;
    }

    iterator &operator++() {
      ++current_;
      return *this;
    }

    iterator operator++(int) {
      iterator tmp = *this;
      ++(*this);
      return tmp;
    }

    bool operator==(const iterator &other) const {
      return current_ == other.current_;
    }
  };

  auto begin() {
    return iterator(std::begin(this->base_), std::begin(this->base_), period_);
  }

  auto end() {
    return iterator(std::end(this->base_), std::begin(this->base_), period_);
  }

  // Size constraint
  auto size()
    requires std::ranges::sized_range<R>
  {
    return std::ranges::size(this->base_);
  }
};

// -------------------------------------------------------------------------
// Adaptor (Pipe Support)
// -------------------------------------------------------------------------
struct sma_closure {
  int period;
};

inline auto sma(int period) { return sma_closure{period}; }

template <std::ranges::view R> auto operator|(R &&r, sma_closure const &c) {
  return sma_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

template <typename R>
  requires(!std::ranges::view<R>)
auto operator|(R &&r, sma_closure const &c) {
  return sma_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

// -------------------------------------------------------------------------
// Eager Wrapper
// -------------------------------------------------------------------------
template <typename R> std::vector<double> compute_sma(R &&data, int period) {
  auto view = std::forward<R>(data) | sma(period);
  // Materialize
  std::vector<double> result;
  if constexpr (std::ranges::sized_range<R>) {
    result.reserve(std::ranges::size(data));
  }
  for (auto v : view) {
    result.push_back(v);
  }
  return result;
}

} // namespace quanux::indicators
