// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Simple test for RingBuffer<T> to verify SPSC behavior.
// Pushes 1000 integers from one thread, pops from another,
// and verifies the values match.

#include "ring_buffer.h"

#include <iostream>
#include <thread>

int main() {
  bool success = true;

  RingBuffer<int> buffer;
  buffer.Initialize(1024);

  // Pass a lambda function to the producer thread.
  std::thread producer([&]() {
    for (int i = 0; i < 1000; ++i) {
      // Try to push i into the buffer until success.
      while (!buffer.Push(&i, 1)) {
      }
    }
  });

  // Pass a lambda function to the consumer thread.
  std::thread consumer([&]() {
    int value;
    for (int i = 0; i < 1000; ++i) {
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