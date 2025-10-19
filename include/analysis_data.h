// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Thread-safe container for audio analysis metrics.
//
// Stores real-time analysis results (RMS, stereo correlation, frequency
// bandwidth, and FFT spectra for both channels). It is shared between
// AnalysisThread (writer) and Visualizer (reader).
//
// Provides Set() and Get() methods for safe concurrent access using a mutex.
//
// Note: Not copyable or movable due to mutex ownership.

#pragma once

#include <array>
#include <mutex>

#include "analysis_constants.h"

// Thread-safe class for sharing audio analysis data between threads.
class AnalysisData {
 public:
  AnalysisData() = default;
  ~AnalysisData() = default;

  // Class owns a mutex, which is non-copyable and non-movable.
  AnalysisData(const AnalysisData&) = delete;
  AnalysisData& operator=(const AnalysisData&) = delete;
  AnalysisData(AnalysisData&&) = delete;
  AnalysisData& operator=(AnalysisData&&) = delete;

  // Must be called from the analysis thread.
  void Set(float rms, float correlation, float bandwidth,
           const std::array<float, analysis::kFftBinCount>& spectrum_left,
           const std::array<float, analysis::kFftBinCount>& spectrum_right);

  // Must be called from the thread reading the analysis data.
  void Get(float& rms, float& correlation, float& bandwidth,
           std::array<float, analysis::kFftBinCount>& spectrum_left,
           std::array<float, analysis::kFftBinCount>& spectrum_right) const;

 private:
  mutable std::mutex mutex_;  // Mutable to allow const Get().

  float rms_ = 0.0F;
  float correlation_ = 0.0F;
  float bandwidth_ = 0.0F;
  std::array<float, analysis::kFftBinCount> spectrum_left_ = {};
  std::array<float, analysis::kFftBinCount> spectrum_right_ = {};
};
