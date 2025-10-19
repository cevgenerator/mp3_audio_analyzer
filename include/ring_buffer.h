// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Lock-free single-producer, single-consumer (SPSC) ring buffer.
//
// Designed for low-latency real-time data transfer between threads,
// using atomics with relaxed and acquire-release memory orderings.
// Wraparound behavior is handled efficiently using a power-of-two buffer size.
//
// IMPORTANT: This class is NOT thread-safe for multiple producers or consumers.
// Only one thread may call Push(), and only one thread may call Pop().
// Violating this will cause undefined behavior.

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
  ~RingBuffer() = default;

  // SPSC ring buffer should be non-copyable, but can be moved.
  RingBuffer(const RingBuffer&) = delete;
  RingBuffer& operator=(const RingBuffer&) = delete;
  RingBuffer(RingBuffer&&) noexcept = default;
  RingBuffer& operator=(RingBuffer&&) noexcept = default;

  // Initialize() must be called right after the constructor.
  [[nodiscard]] bool Initialize(size_t capacity) {
    // Capacity must be a power of two and non-zero.
    // Power-of-two sizing enables efficient wraparound through subtracting 1
    // and using the  bitwise AND operator.
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

    // Load head_ with relaxed: producer only reads its own updates.
    // Load tail_ with acquire: prevents stale tail_ value and reordering before
    // this load.
    size_t head = head_.load(std::memory_order_relaxed);
    size_t tail = tail_.load(std::memory_order_acquire);

    size_t free_space = capacity_ - (head - tail);

    if (count > free_space) {
      std::cerr << "Error: Not enough free space in ring buffer.\n";

      return false;
    }

    size_t index = head & (capacity_ - 1);  // Calculate write index.

    // Determine how many items can be written before wraparound is needed.
    size_t first_copy_count = std::min(count, capacity_ - index);

    // Copy the first chunk directly from data into the buffer at index.
    std::copy_n(data, first_copy_count, &buffer_[index]);

    // If wraparound is needed, write the remaining data to the beginning of the
    // buffer.
    std::copy_n(data + first_copy_count, count - first_copy_count,
                buffer_.data());

    // Store head_ with release: ensures the memory copy is visible to the
    // consumer before it reads this new head value.
    head_.store(head + count, std::memory_order_release);

    return true;
  }

  // Copies `count` items from buffer to destination.
  [[nodiscard]] bool Pop(T* dest, size_t count) {
    if (dest == nullptr || count == 0) {
      std::cerr << "Error: Invalid input for RingBuffer::Pop().\n";

      return false;
    }

    // Load tail_ with relaxed: consumer only reads its own updates.
    // Load head_ with acquire: ensures prior writes by the producer (e.g. to
    // buffer_) are visible before this read.
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

    // Store tail_ with release: ensures all prior consumer operations
    // (including reading from buffer_) happen-before a producer's acquire load
    // of tail_.
    tail_.store(tail + count, std::memory_order_release);

    return true;
  }

  [[nodiscard]] bool Empty() const { return Size() == 0; }

  [[nodiscard]] bool Full() const { return Size() == capacity_; }

  [[nodiscard]] size_t capacity() const { return capacity_; }

  [[nodiscard]] size_t Size() const {
    // Use acquire to ensure this reflects the most recent state from both
    // producer and consumer.
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
