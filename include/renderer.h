// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Renderer class. Handles OpenGL state, bar geometry, and
// drawing.

#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "analysis_data.h"

namespace {

constexpr size_t kNumBands = 32;

}  // namespace

class Renderer {
 public:
  Renderer();
  ~Renderer();

  bool Initialize(long sample_rate,
                  const std::shared_ptr<AnalysisData>& analysis_data);
  void Render();

 private:
  static bool InitializeOpenglState();
  void Update();
  void SmoothBandMagnitudes();
  bool BuildBinToBandMapping();
  bool CreateBarGeometry();
  bool CreateDiamondGeometry();
  void RenderBar(size_t index, float magnitude, bool is_left) const;
  void RenderDiamond(float rms, float correlation, float bandwidth) const;

  float sample_rate_ = 0;

  // Graphics
  glm::mat4 projection_matrix_;  // Initialized in Initialize();
  GLint model_location_ = 0;
  GLint color_location_ = 0;
  GLint projection_location_ = 0;
  GLuint shader_program_ = 0;
  GLuint bar_vao_ = 0;
  GLuint bar_vbo_ = 0;
  GLuint diamond_vao_ = 0;
  GLuint diamond_vbo_ = 0;

  // Audio metrics
  std::shared_ptr<AnalysisData> analysis_data_;
  float rms_ = 0.0F;
  float bandwidth_ = 0.0F;
  float correlation_ = 0.0F;
  std::array<float, analysis::kFftBinCount> spectrum_left_ = {};
  std::array<float, analysis::kFftBinCount> spectrum_right_ = {};

  // Bin to band mapping
  std::array<float, analysis::kFftBinCount> bin_frequencies_ = {};
  std::vector<float> band_edges_ = std::vector<float>(kNumBands + 1, 0.0F);
  std::vector<size_t> bin_to_band_ =
      std::vector<size_t>(analysis::kFftBinCount, 0);
  std::array<float, kNumBands> band_magnitudes_left_ = {};
  std::array<float, kNumBands> band_magnitudes_right_ = {};
};
