#pragma once
#include <atomic>
#include <cstddef>
#include <vector>

template <typename T, size_t Size> class RingBuffer {
public:
  RingBuffer() : head_(0), tail_(0) { buffer_.resize(Size); }

  bool push(const T &item) {
    const auto current_tail = tail_.load(std::memory_order_relaxed);
    const auto next_tail = (current_tail + 1) % Size;

    if (next_tail == head_.load(std::memory_order_acquire)) {
      return false; // Full
    }

    buffer_[current_tail] = item;
    tail_.store(next_tail, std::memory_order_release);
    return true;
  }

  bool pop(T &item) {
    const auto current_head = head_.load(std::memory_order_relaxed);

    if (current_head == tail_.load(std::memory_order_acquire)) {
      return false; // Empty
    }

    item = buffer_[current_head];
    head_.store((current_head + 1) % Size, std::memory_order_release);
    return true;
  }

private:
  std::vector<T> buffer_;
  std::atomic<size_t> head_;
  std::atomic<size_t> tail_;
};
