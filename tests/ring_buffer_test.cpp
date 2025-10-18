// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Simple test for RingBuffer<T> to verify SPSC behavior.
// Pushes 1000 integers from one thread, pops from another,
// and verifies the values match.

#include "ring_buffer.h"

#include <cstddef>
#include <iostream>
#include <thread>

namespace {

constexpr size_t kBufferSize = 1024;
constexpr int kNumberOfIntegers = 1000;

}  // namespace

int main() {
  bool success = true;

  RingBuffer<int> buffer;

  if (!buffer.Initialize(kBufferSize)) {
    std::cerr << "Failed to initialize buffer\n";
    return 1;
  }

  // Pass a lambda function to the producer thread.
  std::thread producer([&]() {
    for (int i = 0; i < kNumberOfIntegers; ++i) {
      // Try to push i into the buffer until success.
      while (!buffer.Push(&i, 1)) {
      }
    }
  });

  // Pass a lambda function to the consumer thread.
  std::thread consumer([&]() {
    int value;
    for (int i = 0; i < kNumberOfIntegers; ++i) {
      // Try to copy 1 value from buffer into `value` until success.
      while (!buffer.Pop(&value, 1)) {
      }

      if (value != i) {
        std::cerr << "Mismatch: expected " << i << ", got " << value << '\n';

        success = false;
      }
    }
  });

  producer.join();
  consumer.join();

  std::cout << (success ? "Test passed.\n" : "Test failed.\n");

  return 0;
}