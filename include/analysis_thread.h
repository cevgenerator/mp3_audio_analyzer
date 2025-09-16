// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Declaration of AnalysisThread class. Creates a thread for reading audio data
// from a ring buffer and performing analysis using FFTW.

#pragma once

#include <atomic>
#include <thread>

#include "ring_buffer.h"

// Initialize() must be called right after the constructor.
class AnalysisThread {
 public:
  AnalysisThread();
  ~AnalysisThread();

  bool Initialize();

  RingBuffer<float>& buffer();  // So producer can write into it.

 private:
  void Start();
  void Stop();
  void Run();

  std::thread thread_;
  std::atomic<bool> running_;
  RingBuffer<float> buffer_;
};
