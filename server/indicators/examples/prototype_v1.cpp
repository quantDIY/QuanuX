#include <algorithm>
#include <cmath>
#include <concepts>
#include <iostream>
#include <numeric>
#include <ranges>
#include <vector>

// namespace alias for brevity
#include <type_traits>

namespace quanux::indicators {

// -------------------------------------------------------------------------
// 1. Concept Definition
// -------------------------------------------------------------------------

// An indicator "View" must leverage C++20 ranges
// It takes a range R and produces values of type T
template <typename T>
concept Numeric = std::is_arithmetic_v<T>;

// -------------------------------------------------------------------------
// 2. The Core "Lazy" SMA View
// -------------------------------------------------------------------------

// Using a simple closure object to allow syntax: input | sma(n)
struct sma_closure {
  int period;
};

// The actual adaptor factory
auto sma(int period) { return sma_closure{period}; }

// The View implementation (simplified for range adaptors)
// In a full implementation, this would inherit from std::ranges::view_interface
template <std::ranges::view R>
class sma_view : public std::ranges::view_interface<sma_view<R>> {
private:
  R base_;
  int period_;

public:
  sma_view() = default;
  constexpr sma_view(R base, int period)
      : base_(std::move(base)), period_(period) {}

  // Iterator implementation details would go here
  // For this prototype, we'll implement a simplified "transform" style behavior
  // Real-world: needs a rolling window implementation

  // This is a naive implementation for the POC to demonstrate syntax
  // Real implementation requires robust iterator categories

  struct iterator {
    using difference_type = std::ptrdiff_t;
    using value_type = double;

    std::ranges::iterator_t<R> current_;
    std::ranges::iterator_t<R> begin_;
    int period_;

    // Standard iterator requirements
    value_type operator*() const {
      // LAZY CALCULATION HAPPENS HERE!
      // We look backwards 'period_' times to compute the average
      // This only runs when the user accesses the data

      if (std::distance(begin_, current_) < period_ - 1) {
        return std::numeric_limits<double>::quiet_NaN();
      }

      double sum = 0.0;
      auto it = current_;
      for (int i = 0; i < period_; ++i) {
        sum += *it;
        if (i < period_ - 1)
          --it;
      }
      return sum / period_;
    }

    iterator &operator++() {
      ++current_;
      return *this;
    }

    void operator++(int) { ++current_; }

    bool operator==(const iterator &other) const {
      return current_ == other.current_;
    }

    bool operator!=(const iterator &other) const { return !(*this == other); }
  };

  auto begin() {
    return iterator{std::begin(base_), std::begin(base_), period_};
  }

  auto end() { return iterator{std::end(base_), std::begin(base_), period_}; }
};

// -------------------------------------------------------------------------
// 3. Pipe Operator Overload
// -------------------------------------------------------------------------

template <std::ranges::view R> auto operator|(R &&r, sma_closure const &c) {
  return sma_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

// Overload for non-view containers (like std::vector) to adapt them
// automatically
template <typename R>
  requires(!std::ranges::view<R>)
auto operator|(R &&r, sma_closure const &c) {
  return sma_view<std::views::all_t<R>>(std::views::all(std::forward<R>(r)),
                                        c.period);
}

} // namespace quanux::indicators

namespace qi = quanux::indicators;

// -------------------------------------------------------------------------
// Demo Execution
// -------------------------------------------------------------------------

int main() {
  // 1. Source Data
  std::vector<double> prices = {100.0, 101.0, 102.0, 103.0, 104.0,
                                105.0, 100.0, 90.0,  85.0,  95.0};

  std::cout << "Original Prices: ";
  for (auto p : prices)
    std::cout << p << " ";
  std::cout << "\n\n";

  // 2. The Indicator Pipeline
  // Syntax: prices | sma(3)
  // Note: This line performs NO calculations. It just creates the view.
  auto ma3 = prices | quanux::indicators::sma(3);

  std::cout << "Calculated SMA(3) Lazily:\n";

  // 3. Iteration (Triggers Calculation)
  int index = 0;
  for (auto val : ma3) {
    std::cout << "Index " << index++ << ": " << val << "\n";
  }

  return 0;
}
