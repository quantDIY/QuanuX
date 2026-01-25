#pragma once

#include "quanux/indicators/concepts.hpp"
#include "quanux/indicators/view.hpp"
#include <cmath>

// Community namespaces might be messy, so we encourage wrapping
namespace community::test {

struct rsi_closure {
  int period;
};

inline auto rsi(int period) { return rsi_closure{period}; }

// A dummy RSI view that just returns 50.0 for testing inclusion
template <std::ranges::view R>
class rsi_view : public quanux::indicators::indicator_view<rsi_view<R>, R> {
private:
  int period_;

public:
  rsi_view() = default;
  constexpr rsi_view(R base, int period)
      : quanux::indicators::indicator_view<rsi_view<R>, R>(std::move(base)),
        period_(period) {}

  // Iterator stub
  class iterator {
  public: // Boilerplate
    using iterator_category = std::forward_iterator_tag;
    using value_type = double;
    using difference_type = std::ptrdiff_t;
    using pointer = const double *;
    using reference = double;

  private:
    std::ranges::iterator_t<R> current_;
    std::ranges::iterator_t<R> end_;

  public:
    constexpr iterator(std::ranges::iterator_t<R> current,
                       std::ranges::iterator_t<R> end)
        : current_(current), end_(end) {}

    value_type operator*() const { return 50.0; } // Dummy value

    iterator &operator++() {
      ++current_;
      return *this;
    }
    iterator operator++(int) {
      auto tmp = *this;
      ++(*this);
      return tmp;
    }
    bool operator==(const iterator &other) const {
      return current_ == other.current_;
    }
  };

  auto begin() {
    return iterator(std::begin(this->base_), std::end(this->base_));
  }
  auto end() { return iterator(std::end(this->base_), std::end(this->base_)); }
};

template <std::ranges::view R> auto operator|(R &&r, rsi_closure const &c) {
  return rsi_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

template <typename R>
  requires(!std::ranges::view<R>)
auto operator|(R &&r, rsi_closure const &c) {
  return rsi_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

} // namespace community::test
