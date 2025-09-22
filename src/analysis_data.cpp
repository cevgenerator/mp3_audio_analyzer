// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AnalysisData class.

#include "analysis_data.h"

AnalysisData::AnalysisData() {}
AnalysisData::~AnalysisData() {}

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
