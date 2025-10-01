// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Renderer class.

#include "renderer.h"

#include <GLFW/glfw3.h>

#include <glm/gtc/matrix_transform.hpp>

#include "error_handling.h"
#include "shader_util.h"
#include "window_constants.h"

namespace {

// Vertices
constexpr int kNumRectangleVertices = 6;
constexpr int kNumLineVertices = 4;

// Outlines
constexpr float kOutlineColorValue = 0.5F;
constexpr float kOutlineWidth = 3.0F;

// Spacing
constexpr float kHorizontalRange = 0.8F;
constexpr float kHorizontalMargin = (1.0F - kHorizontalRange) / 2.0F;
constexpr float kVerticalRange = kHorizontalRange;
constexpr float kVerticalMargin = kHorizontalMargin;

// FFT bars
constexpr float kBarWidth = kHorizontalRange / kNumBands;
constexpr float kBarHeight = 0.05F;
constexpr float kBarAlphaOffset = 0.1F;

// RMS bar
constexpr float kRmsBarScaleFactor = 28.0F;
constexpr float kRmsBarColorValue = 0.5F;

// Diamond shape
constexpr float kUpperBandEdge = 20000.0F;
constexpr float kBandwidthScaleFactor = 3.0F;
constexpr float kCorrelationScaleFactor = 0.8F;
constexpr float kColorScaleFactor = 1.5F;
constexpr float kTranslationFactor = -0.5F;

// Lines
constexpr float kLineWidth = 0.015F;
constexpr float kLineColorValue = 0.25F;
constexpr float kLineAngle = 90.0F;
constexpr float kLineRotationScaleFactor = 1.75F;

// Bin to band mapping
constexpr float kLowerBandEdge = 20.0F;
constexpr float kLogBase10 = 10.0F;
constexpr std::array<float, 7> kSmoothingKernel = {0.05F, 0.1F, 0.2F, 0.3F,
                                                   0.2F,  0.1F, 0.05F};
constexpr int kKernelRadius = 3;  // 7-point kernel: radius 3.

}  // namespace

Renderer::Renderer() {}

Renderer::~Renderer() {
  if (shader_program_ != 0) {
    glDeleteProgram(shader_program_);
  }

  if (bar_vbo_ != 0) {
    glDeleteBuffers(1, &bar_vbo_);
  }

  if (bar_vao_ != 0) {
    glDeleteVertexArrays(1, &bar_vao_);
  }

  if (diamond_vbo_ != 0) {
    glDeleteBuffers(1, &diamond_vbo_);
  }

  if (diamond_vao_ != 0) {
    glDeleteVertexArrays(1, &diamond_vao_);
  }

  if (line_vbo_ != 0) {
    glDeleteBuffers(1, &line_vbo_);
  }

  if (line_vao_ != 0) {
    glDeleteVertexArrays(1, &line_vao_);
  }
}

bool Renderer::Initialize(long sample_rate,
                          const std::shared_ptr<AnalysisData>& analysis_data) {
  sample_rate_ = static_cast<float>(sample_rate);
  analysis_data_ = analysis_data;

  if (!Succeeded("Building bin-to-band mapping", (!BuildBinToBandMapping()))) {
    return false;
  }

  if (!Succeeded("Initializing OpenGL state", (!InitializeOpenglState()))) {
    return false;
  }

  auto program = CreateShaderProgram("shaders/bar.vert", "shaders/bar.frag");

  if (!Succeeded("Creating shader program", (!program))) {
    return false;
  }

  shader_program_ = *program;

  if (!Succeeded("Storing shader program", (shader_program_ == 0))) {
    return false;
  }

  if (!Succeeded("Creating bar geometry", (!CreateBarGeometry()))) {
    return false;
  }

  if (!Succeeded("Creating diamond geometry", (!CreateDiamondGeometry()))) {
    return false;
  }

  if (!Succeeded("Creating Line Geometry", (!CreateLineGeometry()))) {
    return false;
  }

  model_location_ = glGetUniformLocation(shader_program_, "model");

  if (!Succeeded("Getting model uniform location", (model_location_ == -1))) {
    return false;
  }

  projection_matrix_ = glm::ortho(-1.0F, 1.0F, -1.0F, 1.0F);
  projection_location_ = glGetUniformLocation(shader_program_, "projection");

  if (!Succeeded("Getting projection uniform location",
                 (projection_location_ == -1))) {
    return false;
  }

  color_location_ = glGetUniformLocation(shader_program_, "color_uniform");

  return Succeeded("Getting color uniform location", (color_location_ == -1));
}

void Renderer::Render() {
  // Clear screen before drawing new frame.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  glUseProgram(shader_program_);

  // Set the projection matrix.
  glUniformMatrix4fv(projection_location_, 1, GL_FALSE,
                     &projection_matrix_[0][0]);  // Send as uniform.

  // Get analysis data.
  Update();

  // Draw a bar for each band.
  glBindVertexArray(bar_vao_);
  for (size_t i = 0; i < kNumBands; ++i) {
    RenderBar(i, band_magnitudes_left_[i], true);
    RenderBar(i, band_magnitudes_right_[i], false);
  }

  // Draw RMS bar.
  RenderRmsBar(rms_);

  // Draw diamond.
  glBindVertexArray(diamond_vao_);
  RenderDiamond(rms_, correlation_, bandwidth_);

  // Draw lines.
  glBindVertexArray(line_vao_);
  RenderGraphOverlay();

  glBindVertexArray(0);
  glUseProgram(0);
}

// ----------------------
// Private methods
// ----------------------

bool Renderer::InitializeOpenglState() {
  if (!Succeeded("Initializing GLAD",
                 (gladLoadGL((GLADloadfunc)glfwGetProcAddress)) == 0)) {
    return false;
  }

  glViewport(0, 0, window::kWindowWidth, window::kWindowHeight);

  // Set background to black.
  glClearColor(0.0F, 0.0F, 0.0F, 1.0F);

  // Enable blending.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable depth testing.
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}

void Renderer::Update() {
  analysis_data_->Get(rms_, correlation_, bandwidth_, spectrum_left_,
                      spectrum_right_);

  AggregateBins();
  SmoothBandMagnitudes();
}

void Renderer::AggregateBins() {
  // Clear band_magnitudes_.
  band_magnitudes_left_.fill(0.0F);
  band_magnitudes_right_.fill(0.0F);

  // Aggregate bins per band.
  std::array<size_t, kNumBands> bin_counts = {};

  for (size_t bin = 0; bin < analysis::kFftBinCount; ++bin) {
    size_t band = bin_to_band_[bin];

    band_magnitudes_left_[band] += spectrum_left_[bin];
    band_magnitudes_right_[band] += spectrum_right_[bin];
    bin_counts[band]++;
  }

  // Normalize.
  for (size_t band = 0; band < kNumBands; ++band) {
    if (bin_counts[band] > 0) {
      band_magnitudes_left_[band] /= static_cast<float>(bin_counts[band]);
      band_magnitudes_right_[band] /= static_cast<float>(bin_counts[band]);
    }
  }
}

void Renderer::SmoothBandMagnitudes() {
  std::array<float, kNumBands> smoothed_left = {};
  std::array<float, kNumBands> smoothed_right = {};

  for (size_t i = 0; i < kNumBands; ++i) {
    float weighted_sum_left = 0.0F;
    float weighted_sum_right = 0.0F;
    float total_weight = 0.0F;  // Actual used weight (important at edges).

    // Apply kernel.
    for (int offset = -kKernelRadius; offset <= kKernelRadius; ++offset) {
      int neighbor_index = static_cast<int>(i) + offset;  // Which band.

      // Clamp to valid index range.
      if (neighbor_index >= 0 && neighbor_index < static_cast<int>(kNumBands)) {
        // Find corresponding weight for current band.
        float weight = kSmoothingKernel[offset + kKernelRadius];

        // Add weighted neighbour magnitude to sum.
        weighted_sum_left += band_magnitudes_left_[neighbor_index] * weight;
        weighted_sum_right += band_magnitudes_right_[neighbor_index] * weight;
        total_weight += weight;
      }
    }

    // Normalize (important at edges where total_weight < 1).
    smoothed_left[i] = weighted_sum_left / total_weight;
    smoothed_right[i] = weighted_sum_right / total_weight;
  }

  // Overwrite original magnitudes.
  band_magnitudes_left_ = smoothed_left;
  band_magnitudes_right_ = smoothed_right;
}

bool Renderer::BuildBinToBandMapping() {
  if (!Succeeded("Validating sample rate", (sample_rate_ <= 0.0F))) {
    return false;
  }

  // Calculate bin frequencies.
  for (size_t i = 0; i < analysis::kFftBinCount; ++i) {
    bin_frequencies_[i] =
        static_cast<float>(i) * sample_rate_ / analysis::kFftSize;
  }

  // Define frequency range edges.
  float min_freq = kLowerBandEdge;
  float max_freq = sample_rate_ / 2;  // Nyquist.

  if (!Succeeded("Validating range edges", (min_freq >= max_freq))) {
    return false;
  }

  // Compute logarithmically spaced frequency range edges.
  float log_min = std::log10(min_freq);
  float log_max = std::log10(max_freq);

  // Compute logarithmically spaced band edges.
  for (size_t i = 0; i <= kNumBands; ++i) {
    // Convert i to normalized value (0.0 to 1.0).
    float normalized_edge_position = static_cast<float>(i) / kNumBands;

    // Interpolate evenly in log space and convert back to linear frequency.
    float freq = std::pow(
        kLogBase10, log_min + (normalized_edge_position * (log_max - log_min)));

    band_edges_[i] = freq;
  }

  // Pre-compute bin-to-band mappings.
  for (size_t bin = 0; bin < analysis::kFftBinCount; ++bin) {
    float freq = bin_frequencies_[bin];

    // Find the first band edge that this freq is less than.
    bool matched = false;

    for (size_t band = 0; band < kNumBands; ++band) {
      if (freq < band_edges_[band + 1]) {
        bin_to_band_[bin] = band;
        matched = true;
        break;
      }
    }

    if (!Succeeded("Matching bins to bands", (!matched))) {
      return false;
    }
  }

  return true;
}

// ----------------------
// Bar shape methods
// ----------------------

bool Renderer::CreateBarGeometry() {
  // 2 triangles forming a vertical bar centered on origin.
  float bar_vertices[] = {
      // x, y
      0.0F,      0.0F,       kBarWidth, 0.0F,       kBarWidth, kBarHeight,

      kBarWidth, kBarHeight, 0.0F,      kBarHeight, 0.0F,      0.0F,
  };

  glGenVertexArrays(1, &bar_vao_);
  glGenBuffers(1, &bar_vbo_);

  // Bind the VAO. All following vertex format/state settings are stored in it.
  glBindVertexArray(bar_vao_);

  // Upload vertex data to VBO.
  glBindBuffer(GL_ARRAY_BUFFER, bar_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(bar_vertices), bar_vertices,
               GL_STATIC_DRAW);

  // Describe vertex layout.
  glVertexAttribPointer(
      0,         // Attribute index (matches layout(location = 0) in shader).
      2,         // Components per vertex attribute (x and y).
      GL_FLOAT,  // Type.
      GL_FALSE,  // Normalize.
      2 * sizeof(float),  // Stride (bytes between vertices).
      (void*)0            // Offset.
  );
  glEnableVertexAttribArray(0);  // Link buffer data to shader input.

  // Unbind.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return (bar_vao_ != 0) && (bar_vbo_ != 0);
}

// index and magnitude are clearly named, and this function is only used
// internally.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Renderer::RenderBar(size_t index, float magnitude, bool is_left) const {
  // Compute horizontal position based on index.
  float horizontal_position =
      (kBarWidth * static_cast<float>(index)) + kHorizontalMargin;

  // Compute width based on magnitude.
  float height = magnitude;

  // Shift left bands.
  if (is_left) {
    horizontal_position -= 1.0F;
  }

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::translate(model,
                         glm::vec3(horizontal_position, kVerticalMargin, 0.0F));
  model =
      glm::scale(model, glm::vec3(1.0F, height, 1.0F));  // Vertical scale only.

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  float color_value = static_cast<float>(index) / static_cast<float>(kNumBands);

  glUniform4f(color_location_, color_value, color_value, 1.0F,
              color_value + kBarAlphaOffset);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kNumRectangleVertices);
}

void Renderer::RenderRmsBar(float rms) const {
  float height = rms * kRmsBarScaleFactor;
  float horizontal_position = kHorizontalMargin + ((kBarWidth * kNumBands) / 3);

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::translate(
      model, glm::vec3(horizontal_position, -1.0F + kVerticalMargin, 0.0F));
  model = glm::scale(model, glm::vec3(kNumBands / 3, height, 1.0F));

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  glUniform4f(color_location_, kRmsBarColorValue, 0.0F, 1.0, 1.0F);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kNumRectangleVertices);
}

// ----------------------
// Diamond shape methods
// ----------------------

bool Renderer::CreateDiamondGeometry() {
  // 2 triangles forming a diamond shape centered on origin.
  float diamond_vertices[] = {
      // x, y
      -1.0F, 0.0F, 0.0F, 1.0F,  1.0F,  0.0F,

      1.0F,  0.0F, 0.0F, -1.0F, -1.0F, 0.0F,
  };

  glGenVertexArrays(1, &diamond_vao_);
  glGenBuffers(1, &diamond_vbo_);

  // Bind the VAO. All following vertex format/state settings are stored in it.
  glBindVertexArray(diamond_vao_);

  // Upload vertex data to VBO.
  glBindBuffer(GL_ARRAY_BUFFER, diamond_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(diamond_vertices), diamond_vertices,
               GL_STATIC_DRAW);

  // Describe vertex layout.
  glVertexAttribPointer(
      0,         // Attribute index (matches layout(location = 0) in shader).
      2,         // Components per vertex attribute (x and y).
      GL_FLOAT,  // Type.
      GL_FALSE,  // Normalize.
      2 * sizeof(float),  // Stride (bytes between vertices).
      (void*)0            // Offset.
  );
  glEnableVertexAttribArray(0);  // Link buffer data to shader input.

  // Unbind.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return (diamond_vao_ != 0) && (diamond_vbo_ != 0);
}

// rms, correlation and bandwidth are clearly named, and this function is only
// used internally.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Renderer::RenderDiamond(float rms, float correlation,
                             float bandwidth) const {
  // Compute height based on bandwidth.
  float height = (bandwidth / kUpperBandEdge) / kBandwidthScaleFactor;

  // Compute width based on correlation.
  float width = correlation * kCorrelationScaleFactor;

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::translate(
      model, glm::vec3(kTranslationFactor, kTranslationFactor, 0.0F));
  model = glm::scale(
      model, glm::vec3(width, height, 1.0F));  // Horizontal and vertical scale.

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  glUniform4f(color_location_, 0.0F, rms, rms / kColorScaleFactor, 1.0F);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kNumRectangleVertices);

  // Draw outline.
  glUniform4f(color_location_, 0.0F, rms + kOutlineColorValue,
              (rms / kColorScaleFactor) + kOutlineColorValue, 1.0F);
  glLineWidth(kOutlineWidth);  // Some drivers ignore values > 1.0.
  glDrawArrays(GL_LINE_LOOP, 0, kNumRectangleVertices);
}

// ----------------------
// Line methods
// ----------------------

bool Renderer::CreateLineGeometry() {
  float line_vertices[] = {
      // x, y
      -kLineWidth, 0.0F,           0.0F,        0.0F,

      0.0F,        kVerticalRange, -kLineWidth, kVerticalRange,
  };

  glGenVertexArrays(1, &line_vao_);
  glGenBuffers(1, &line_vbo_);

  // Bind the VAO. All following vertex format/state settings are stored in it.
  glBindVertexArray(line_vao_);

  // Upload vertex data to VBO.
  glBindBuffer(GL_ARRAY_BUFFER, line_vbo_);
  glBufferData(GL_ARRAY_BUFFER, sizeof(line_vertices), line_vertices,
               GL_STATIC_DRAW);

  // Describe vertex layout.
  glVertexAttribPointer(
      0,         // Attribute index (matches layout(location = 0) in shader).
      2,         // Components per vertex attribute (x and y).
      GL_FLOAT,  // Type.
      GL_FALSE,  // Normalize.
      2 * sizeof(float),  // Stride (bytes between vertices).
      (void*)0            // Offset.
  );
  glEnableVertexAttribArray(0);  // Link buffer data to shader input.

  // Unbind.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return (line_vao_ != 0) && (line_vbo_ != 0);
}

// horizontal_position and vertical_position are clearly named, and this
// function is only used internally.
// NOLINTNEXTLINE(bugprone-easily-swappable-parameters)
void Renderer::RenderLine(bool is_horizontal, float horizontal_position,
                          float vertical_position) const {
  float hor_pos = horizontal_position + kHorizontalMargin;
  float ver_pos = vertical_position + kVerticalMargin - window::kPixelY;

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::translate(model, glm::vec3(hor_pos, ver_pos, 0.0F));

  if (is_horizontal) {
    model = glm::scale(model, glm::vec3(-1.0F, kLineRotationScaleFactor, 1.0F));
    model = glm::rotate(model, glm::radians(kLineAngle),
                        glm::vec3(0.0F, 0.0F, 1.0F));
  }

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  glUniform4f(color_location_, kLineColorValue, kLineColorValue,
              kLineColorValue, 1.0F);

  // Set line width.
  glLineWidth(1.0F);  // Some drivers ignore values > 1.0.

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_LINE_STRIP, 0, kNumLineVertices);
}

// ----------------------
// Overlay methods
// ----------------------

void Renderer::RenderLabels() const {
  //
}

void Renderer::RenderGraphOverlay() const {
  // Y-axes
  RenderLine(false, 0.0F, 0.0F);    // Right FFT.
  RenderLine(false, -1.0F, 0.0F);   // Left FFT.
  RenderLine(false, 0.0F, -1.0F);   // RMS.
  RenderLine(false, -1.0F, -1.0F);  // Diamond.
  // X-axes
  RenderLine(true, 0.0F, 0.0F);    // Right FFT.
  RenderLine(true, -1.0F, 0.0F);   // Left FFT.
  RenderLine(true, 0.0F, -1.0F);   // RMS.
  RenderLine(true, -1.0F, -1.0F);  // Diamond.

  // RenderLabels();
}