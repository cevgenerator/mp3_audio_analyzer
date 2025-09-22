// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of AnalysisThread class. Creates a thread for reading audio data
// from a ring buffer and performing analysis using FFTW.

#pragma once

#include <atomic>
#include <thread>

#include "fftw_wrapper.h"
#include "ring_buffer.h"

// Initialize() must be called right after the constructor.
class AnalysisThread {
 public:
  AnalysisThread();
  ~AnalysisThread();

  bool Initialize(long sample_rate);

  RingBuffer<float>& buffer();  // So producer can write into it.

 private:
  void Start();
  void Stop();
  void CalculateRms();
  float CalculateBandwidth(const fftwf_complex* output) const;
  void CalculateAverageBandwidth();
  void Run();

  std::thread thread_;
  std::atomic<bool> running_;
  RingBuffer<float> buffer_;
  std::vector<float> interleaved_;
  FftwWrapper fft;
  int fft_count_ = 0;
  float rms_left_ = 0.0F;
  float rms_right_ = 0.0F;
  float sample_rate_ = 0;
  float bandwidth_ = 0.0F;
};
