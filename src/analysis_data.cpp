// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AnalysisData class.

#include "analysis_data.h"

// Accepting the tradeoff for now to keep the interface simple.
// Parameter order is clear and usage is consistent across the project.
// May refactor to use a struct if number of parameters increases.
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void AnalysisData::Set(
    float rms, float correlation, float bandwidth,
    const std::array<float, analysis::kFftBinCount>& spectrum_left,
    const std::array<float, analysis::kFftBinCount>& spectrum_right) {
  std::scoped_lock lock(mutex_);
  rms_ = rms;
  correlation_ = correlation;
  bandwidth_ = bandwidth;
  spectrum_left_ = spectrum_left;
  spectrum_right_ = spectrum_right;
}
// NOLINTEND(bugprone-easily-swappable-parameters)

// Accepting the tradeoff for now to keep the interface simple.
// Parameter order is clear and usage is consistent across the project.
// May refactor to use a struct if number of parameters increases.
// NOLINTBEGIN(bugprone-easily-swappable-parameters)
void AnalysisData::Get(
    float& rms, float& correlation, float& bandwidth,
    std::array<float, analysis::kFftBinCount>& spectrum_left,
    std::array<float, analysis::kFftBinCount>& spectrum_right) const {
  std::scoped_lock lock(mutex_);
  rms = rms_;
  correlation = correlation_;
  bandwidth = bandwidth_;
  spectrum_left = spectrum_left_;
  spectrum_right = spectrum_right_;
}
// NOLINTEND(bugprone-easily-swappable-parameters)