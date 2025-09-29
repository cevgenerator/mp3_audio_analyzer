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

#include "analysis_data.h"

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
  bool CreateBarGeometry();
  void RenderBar(size_t index, float magnitude, bool is_left) const;

  float sample_rate_ = 0;

  // Graphics
  glm::mat4 projection_matrix_;  // Initialized in Initialize();
  GLint model_location_ = 0;
  GLint color_location_ = 0;
  GLint projection_location_ = 0;
  GLuint shader_program_ = 0;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;

  // Audio metrics
  std::shared_ptr<AnalysisData> analysis_data_;
  float rms_ = 0.0F;
  float bandwidth_ = 0.0F;
  float correlation_ = 0.0F;
  std::array<float, analysis::kFftBinCount> spectrum_left_ = {};
  std::array<float, analysis::kFftBinCount> spectrum_right_ = {};
  std::array<float, analysis::kFftBinCount> bin_frequencies_ = {};
};