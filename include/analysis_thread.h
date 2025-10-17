// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of AnalysisThread class. Creates a thread for reading audio data
// from a ring buffer and performing analysis using FFTW.

#pragma once

#include <atomic>
#include <memory>
#include <thread>

#include "analysis_data.h"
#include "fftw_wrapper.h"
#include "ring_buffer.h"

// Initialize() must be called right after the constructor.
class AnalysisThread {
 public:
  AnalysisThread();
  ~AnalysisThread();

  [[nodiscard]] bool Initialize(
      long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data);

  [[nodiscard]] RingBuffer<float>& buffer();  // So producer can write into it.

 private:
  void Start();
  void Stop();
  void CalculateRms();
  void CalculateStereoCorrelation();
  [[nodiscard]] float CalculateBandwidth(const fftwf_complex* output) const;
  void CalculateAverageBandwidth();
  void CalculateMagnitudes();
  void Run();

  std::thread thread_;
  std::atomic<bool> running_;
  RingBuffer<float> buffer_;
  std::vector<float> interleaved_;
  FftwWrapper fft_;
  std::shared_ptr<AnalysisData> analysis_data_;
  int fft_count_ = 0;
  float sample_rate_ = 0;
  float rms_ = 0.0F;
  float bandwidth_ = 0.0F;
  float correlation_ = 0.0F;
  std::array<float, analysis::kFftBinCount> spectrum_left_ = {};
  std::array<float, analysis::kFftBinCount> spectrum_right_ = {};
};
