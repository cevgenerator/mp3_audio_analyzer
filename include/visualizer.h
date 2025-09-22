// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Visualizer class.
//
// This class uses OpenGL, GLAD and GLFW to visualize the results of the
// audio analysis.

#pragma once

#include <memory>

#include "analysis_data.h"

class Visualizer {
 public:
  Visualizer();
  ~Visualizer();

  bool Initialize(const std::shared_ptr<AnalysisData>& analysis_data);
  void Run();

 private:
  void Update();

  std::shared_ptr<AnalysisData> analysis_data_;
  float rms_ = 0.0F;
  float bandwidth_ = 0.0F;
  float correlation_ = 0.0F;
  std::array<float, analysis::kFftBinCount> spectrum_left_ = {};
  std::array<float, analysis::kFftBinCount> spectrum_right_ = {};
};