#include <cmath>
#include <concepts>
#include <iostream>
#include <map>
#include <unordered_map>
#include <vector>

// -------------------------------------------------------------------------
// 1. Tick & Policy Concepts
// -------------------------------------------------------------------------

struct Tick {
  double price;
  double volume;
  // timestamp...
};

// Concept for a Storage Policy
template <typename P>
concept StoragePolicy = requires(P p, double price) {
  { p.add_tpo(price) } -> std::same_as<void>;
  { p.get_tpo_count(price) } -> std::same_as<int>;
};

// -------------------------------------------------------------------------
// 2. Concrete Policies (The "Build It All" Menu)
// -------------------------------------------------------------------------

// Option A: Sparse Storage (Low Memory, Map Based)
// Good for: Crypto, huge price ranges, non-contiguous trading
class SparseStorage {
private:
  // Using simple map for prototype, could be absl::flat_hash_map
  // Key: Price (quantized), Value: TPO Count
  std::map<double, int> profile_;
  double tick_size_;

public:
  SparseStorage(double tick_size) : tick_size_(tick_size) {}

  void add_tpo(double price) {
    double level = std::round(price / tick_size_) * tick_size_;
    profile_[level]++;
  }

  int get_tpo_count(double price) const {
    double level = std::round(price / tick_size_) * tick_size_;
    auto it = profile_.find(level);
    if (it != profile_.end())
      return it->second;
    return 0;
  }
};

// Option B: Dense Storage (High Speed, Vector Based)
// Good for: Futures (ES, NQ), tight tick grids, max performance
class DenseStorage {
private:
  std::vector<int> buffer_;
  double base_price_; // The price at index 0
  double tick_size_;

public:
  DenseStorage(double base_price, double tick_size, int initial_size = 1000)
      : base_price_(base_price), tick_size_(tick_size) {
    buffer_.resize(initial_size, 0);
  }

  void add_tpo(double price) {
    int index =
        static_cast<int>(std::round((price - base_price_) / tick_size_));

    // Dynamic resizing (Naively for prototype)
    if (index < 0 || index >= buffer_.size()) {
      // In real world: handle resizing or offset shifting
      std::cout << "[DenseStorage] Warning: Out of bounds (Resizing needed)\n";
      return;
    }
    buffer_[index]++;
  }

  int get_tpo_count(double price) const {
    int index =
        static_cast<int>(std::round((price - base_price_) / tick_size_));
    if (index >= 0 && index < buffer_.size())
      return buffer_[index];
    return 0;
  }
};

// -------------------------------------------------------------------------
// 3. The Market Profile Indicator (Host)
// -------------------------------------------------------------------------

template <typename Policy>
  requires StoragePolicy<Policy>
class MarketProfile {
private:
  Policy storage_;

public:
  // Forward arguments to Policy constructor
  template <typename... Args>
  MarketProfile(Args &&...args) : storage_(std::forward<Args>(args)...) {}

  void process(const Tick &tick) { storage_.add_tpo(tick.price); }

  int query(double price) { return storage_.get_tpo_count(price); }
};

// -------------------------------------------------------------------------
// Demo
// -------------------------------------------------------------------------

int main() {
  Tick t1{99.25, 1};
  Tick t2{100.00, 5};
  Tick t3{100.25, 1};

  // User Selection 1: Sparse
  std::cout << "--- Sparse Profile ---\n";
  MarketProfile<SparseStorage> crypto_profile(0.25); // Tick size
  crypto_profile.process(t1);
  crypto_profile.process(t2);
  crypto_profile.process(t2); // Hit 100.00 again
  std::cout << "TPOs at 100.00: " << crypto_profile.query(100.00) << "\n";

  // User Selection 2: Dense
  std::cout << "\n--- Dense Profile ---\n";
  MarketProfile<DenseStorage> futures_profile(90.00,
                                              0.25); // Base price 90, tick 0.25
  futures_profile.process(t1);
  futures_profile.process(t2);
  futures_profile.process(t3);
  std::cout << "TPOs at 100.00: " << futures_profile.query(100.00) << "\n";

  return 0;
}
