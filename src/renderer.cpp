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

constexpr float kClearColorR = 0.0F;
constexpr float kClearColorG = 0.0F;
constexpr float kClearColorB = 0.0F;
constexpr float kClearColorA = 1.0F;

constexpr int kRectangleVertices = 6;

constexpr float kVerticalRange = 2.0F;  // From -1.0F to 1.0F.
constexpr float kBarWidth = 0.05F;
constexpr float kBarHeight = kVerticalRange / analysis::kFftBinCount;
constexpr float kBarAlphaOffset = 0.1F;

constexpr float kApproxMaxBandwith = 20000.0F;
constexpr float kBandwidthScaleFactor = 1.5F;
constexpr float kCorrelationScaleFactor = 2.0F;

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
}

bool Renderer::Initialize(long sample_rate,
                          const std::shared_ptr<AnalysisData>& analysis_data) {
  sample_rate_ = static_cast<float>(sample_rate);
  analysis_data_ = analysis_data;

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

  // Draw a bar for each bin.
  glBindVertexArray(bar_vao_);
  for (size_t i = 0; i < analysis::kFftBinCount; ++i) {
    RenderBar(i, spectrum_left_[i], true);
    RenderBar(i, spectrum_right_[i], false);
  }

  // Draw diamond.
  glBindVertexArray(diamond_vao_);
  RenderDiamond(rms_, correlation_, bandwidth_);

  glBindVertexArray(0);
  glUseProgram(0);
}

bool Renderer::InitializeOpenglState() {
  if (!Succeeded("Initializing GLAD",
                 (gladLoadGL((GLADloadfunc)glfwGetProcAddress)) == 0)) {
    return false;
  }

  glViewport(0, 0, window::kWindowWidth, window::kWindowHeight);

  // Set background to black.
  glClearColor(kClearColorR, kClearColorG, kClearColorB, kClearColorA);

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
}

bool Renderer::CreateBarGeometry() {
  // 2 triangles forming a vertical bar centered on origin.
  float bar_vertices[] = {
      // x, y
      -kBarWidth / 2, 0.0F,           kBarWidth / 2,
      0.0F,           kBarWidth / 2,  kBarHeight,

      kBarWidth / 2,  kBarHeight,     -kBarWidth / 2,
      kBarHeight,     -kBarWidth / 2, 0.0F,
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
  // Compute vertical position based on index.
  float vertical_position = (kBarHeight * static_cast<float>(index)) - 1.0F;

  // Compute width based on magnitude.
  float width = magnitude;

  // Apply direction (left/right).
  if (is_left) {
    width *= -1.0F;
  }

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::translate(model, glm::vec3(0.0F, vertical_position, 0.0F));
  model = glm::scale(model,
                     glm::vec3(width, 1.0F, 1.0F));  // Horizontal scale only.

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  float color_value =
      (1.0F / analysis::kFftBinCount) * static_cast<float>(index);
  glUniform4f(color_location_, color_value, color_value, 1.0F,
              color_value + kBarAlphaOffset);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kRectangleVertices);
}

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
  float height = (bandwidth / kApproxMaxBandwith) / kBandwidthScaleFactor;

  // Compute width based on correlation.
  float width = correlation * kCorrelationScaleFactor;

  // Build the model matrix.
  glm::mat4 model = glm::mat4(1.0F);  // Identity.
  model = glm::scale(
      model, glm::vec3(width, height, 1.0F));  // Horizontal and vertical scale.

  // Set the model location.
  glUniformMatrix4fv(model_location_, 1, GL_FALSE, &model[0][0]);

  // Set the color uniform.
  glUniform4f(color_location_, rms / kBandwidthScaleFactor, 0.0F, rms, 1.0F);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kRectangleVertices);
}
