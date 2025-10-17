// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of AnalysisData class.
// This class contains the shared data between AnalysisThread and Visualizer.

#pragma once

#include <array>
#include <mutex>

#include "analysis_constants.h"

class AnalysisData {
 public:
  AnalysisData() = default;
  ~AnalysisData() = default;

  void Set(float rms, float correlation, float bandwidth,
           const std::array<float, analysis::kFftBinCount>& spectrum_left,
           const std::array<float, analysis::kFftBinCount>& spectrum_right);

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