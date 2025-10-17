// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Lock-free, single-producer single-consumer (SPSC) ring buffer.
//
// This class implements a circular buffer using atomic operations,
// suitable for audio or real-time data streaming applications.
// It avoids locks by relying on relaxed memory ordering and a
// fixed-capacity design. Wraparound behavior is handled efficiently
// using a power-of-two buffer size.
//
// Designed for low-latency communication between a producer thread
// (e.g. decoder) and a consumer thread (e.g. analyzer).

#pragma once

#include <algorithm>
#include <atomic>
#include <cstddef>
#include <iostream>
#include <vector>

// RingBuffer<T> is a lock-free, fixed-size circular buffer for single-producer,
// single-consumer (SPSC) use cases.
//
// This class is not thread-safe for multiple producers or consumers.
// Both Push() and Pop() are non-blocking and return false if the operation
// would overflow/underflow the buffer.
//
// Requires T to be trivially copyable.
template <typename T>
class RingBuffer {
  static_assert(std::is_trivially_copyable_v<T>,
                "RingBuffer<T> requires trivially copyable type");

 public:
  RingBuffer() = default;

  // Initialize() must be called right after the constructor.
  [[nodiscard]] bool Initialize(size_t capacity) {
    // Must be a power of two and non-zero.
    if (capacity == 0 || (capacity & (capacity - 1)) != 0) {
      return false;
    }

    capacity_ = capacity;
    buffer_.resize(capacity_);

    return true;
  }

  // Pushes `count` items into the ring buffer. Returns false if not enough
  // space.
  [[nodiscard]] bool Push(const T* data, size_t count) {
    if (data == nullptr || count == 0) {
      std::cerr << "Error: Invalid input for RingBuffer::Push().\n";

      return false;
    }

    // Load head and tail atomically.
    size_t head = head_.load(std::memory_order_relaxed);
    size_t tail = tail_.load(std::memory_order_acquire);

    size_t free_space = capacity_ - (head - tail);

    if (count > free_space) {
      std::cerr << "Error: Not enough free space in ring buffer.\n";

      return false;
    }

    size_t index = head & (capacity_ - 1);  // Calculate write position.

    // Determine how many items can be written before wraparound is needed.
    size_t first_copy_count = std::min(count, capacity_ - index);

    // Copy the first chunk directly from data into the buffer at index.
    std::copy_n(data, first_copy_count, &buffer_[index]);

    // If wraparound is needed, write the remaining data to the beginning of the
    // buffer.
    std::copy_n(data + first_copy_count, count - first_copy_count,
                buffer_.data());

    // Update head_ atomically.
    // `release` ensures the memory copy is visible to the consumer before it
    // reads this new head value.
    head_.store(head + count, std::memory_order_release);

    return true;
  }

  // Copies `count` items from buffer to destination.
  [[nodiscard]] bool Pop(T* dest, size_t count) {
    if (dest == nullptr || count == 0) {
      std::cerr << "Error: Invalid input for RingBuffer::Pop().\n";

      return false;
    }

    // Load tail and head atomically.
    size_t tail = tail_.load(std::memory_order_relaxed);
    size_t head = head_.load(std::memory_order_acquire);

    // Calculate how many items are available to read.
    size_t used = head - tail;

    if (count > used) {
      return false;  // Not enough data.
    }

    // Calculate read index (wraparound-safe).
    size_t index = tail & (capacity_ - 1);
    size_t first_copy_count = std::min(count, capacity_ - index);

    // Copy the first segment.
    std::copy_n(&buffer_[index], first_copy_count, dest);

    // Copy the second segment, if wrapping is needed.
    std::copy_n(buffer_.data(), count - first_copy_count,
                dest + first_copy_count);

    // Update tail_ atomically (release ensures memory copy is visible).
    tail_.store(tail + count, std::memory_order_release);

    return true;
  }

  [[nodiscard]] bool Empty() const { return Size() == 0; }

  [[nodiscard]] bool Full() const { return Size() == capacity_; }

  [[nodiscard]] size_t capacity() const { return capacity_; }

  [[nodiscard]] size_t Size() const {
    size_t head = head_.load(std::memory_order_acquire);
    size_t tail = tail_.load(std::memory_order_acquire);

    return head - tail;
  }

 private:
  std::vector<T> buffer_;
  std::atomic<size_t> head_ = 0;
  std::atomic<size_t> tail_ = 0;
  size_t capacity_ = 0;
};