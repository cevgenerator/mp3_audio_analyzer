// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Renderer class. Handles OpenGL state, shape, line and font
// texture geometry, as well as drawing. Is used and owned by Visualizer to
// render a real-time visualization of the analysis data.

#pragma once

#include <glad/gl.h>

#include <cstddef>
#include <glm/glm.hpp>
#include <memory>
#include <vector>

#include "analysis_data.h"
#include "font_atlas.h"

namespace {

constexpr size_t kNumBands = 32;

}  // namespace

class Renderer {
 public:
  Renderer() = default;
  ~Renderer();

  // Non-copyable for safety, non-movable for simplicity.
  Renderer(const Renderer&) = delete;
  Renderer& operator=(const Renderer&) = delete;
  Renderer(Renderer&&) = delete;
  Renderer& operator=(Renderer&&) = delete;

  // Initialize() must be called right after the constructor.
  [[nodiscard]] bool Initialize(
      long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data);
  void Render();

 private:
  [[nodiscard]] static bool InitializeOpenglState();
  void Update();

  // Bin to band mapping
  void AggregateBins();
  void SmoothBandMagnitudes();
  [[nodiscard]] bool BuildBinToBandMapping();

  // Geometry
  [[nodiscard]] bool CreateBarGeometry();
  [[nodiscard]] bool CreateDiamondGeometry();
  [[nodiscard]] bool CreateLineGeometry();
  [[nodiscard]] bool CreateLabelGeometry();

  // Rendering
  void RenderBar(size_t index, float magnitude, bool is_left) const;
  void RenderRmsBar(float rms) const;
  void RenderDiamond(float rms, float correlation, float bandwidth) const;
  void RenderLine(bool is_horizontal, float horizontal_position,
                  float vertical_position) const;
  void RenderLabels() const;
  void RenderGraphOverlay() const;

  // Graphics
  glm::mat4 projection_matrix_;  // Initialized in Initialize();

  GLint model_location_ = 0;
  GLint color_location_ = 0;
  GLint projection_location_ = 0;
  GLint text_projection_location_ = 0;
  GLint text_model_location_ = 0;
  GLint text_color_location_ = 0;

  GLuint shader_program_ = 0;
  GLuint text_shader_program_ = 0;
  GLuint bar_vao_ = 0;
  GLuint bar_vbo_ = 0;
  GLuint diamond_vao_ = 0;
  GLuint diamond_vbo_ = 0;
  GLuint line_vao_ = 0;
  GLuint line_vbo_ = 0;
  GLuint label_vao_ = 0;
  GLuint label_vbo_ = 0;

  FontAtlas font_atlas_;
  GLsizei label_vertex_count_ = 0;

  // Audio metrics
  float sample_rate_ = 0;
  std::shared_ptr<AnalysisData> analysis_data_ = nullptr;
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
