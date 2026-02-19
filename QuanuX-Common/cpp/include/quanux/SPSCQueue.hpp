#pragma once

#include <atomic>
#include <cstddef>
#include <optional>
#include <vector>

namespace quanux {

/**
 * @brief Lock-Free Single-Producer Single-Consumer Queue
 *
 * Uses a ring buffer with atomic head/tail indices.
 * Optimized for cache line separation to prevent false sharing between producer
 * and consumer.
 */
template <typename T> class SPSCQueue {
public:
  explicit SPSCQueue(size_t capacity)
      : capacity_(capacity), data_(capacity + 1) {}

  /**
   * @brief Push an item into the queue.
   * @return true if successful, false if full.
   */
  bool push(const T &item) {
    const size_t current_tail = tail_.load(std::memory_order_relaxed);
    const size_t next_tail = (current_tail + 1) % data_.size();

    if (next_tail != head_.load(std::memory_order_acquire)) {
      data_[current_tail] = item;
      tail_.store(next_tail, std::memory_order_release);
      return true;
    }
    return false;
  }

  /**
   * @brief Pop an item from the queue.
   * @param item Reference to store the popped item.
   * @return true if successful, false if empty.
   */
  bool pop(T &item) {
    const size_t current_head = head_.load(std::memory_order_relaxed);

    if (current_head == tail_.load(std::memory_order_acquire)) {
      return false; // Empty
    }

    item = data_[current_head];
    head_.store((current_head + 1) % data_.size(), std::memory_order_release);
    return true;
  }

private:
  size_t capacity_;
  std::vector<T> data_;

  // Align head and tail to separate cache lines to prevent false sharing
  alignas(64) std::atomic<size_t> head_{0};
  alignas(64) std::atomic<size_t> tail_{0};
};

} // namespace quanux
