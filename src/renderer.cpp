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

constexpr int kBarVertices = 6;
constexpr float kBarWidth = 0.05F;
constexpr float kBarHeight = 2.0F / analysis::kFftBinCount;

}  // namespace

Renderer::Renderer() {}

Renderer::~Renderer() {
  if (shader_program_ != 0) {
    glDeleteProgram(shader_program_);
  }

  if (vbo_ != 0) {
    glDeleteBuffers(1, &vbo_);
  }

  if (vao_ != 0) {
    glDeleteVertexArrays(1, &vao_);
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
  };

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
  glBindVertexArray(vao_);

  // Set the projection matrix.
  glUniformMatrix4fv(projection_location_, 1, GL_FALSE,
                     &projection_matrix_[0][0]);  // Send as uniform.

  // Get analysis data.
  Update();

  // Draw a bar for each bin.
  for (size_t i = 0; i < analysis::kFftBinCount; ++i) {
    RenderBar(i, spectrum_left_[i], true);
    RenderBar(i, spectrum_right_[i], false);
  }

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

  // Create and bind VAO and VBO.
  GLuint vao;
  GLuint vbo;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &vbo);

  // Bind the VAO. All following vertex format/state settings are stored in it.
  glBindVertexArray(vao);

  // Upload vertex data to VBO.
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
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

  // Assign.
  vao_ = vao;
  vbo_ = vbo;

  // Unbind.
  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  return (vao_ != 0) && (vbo_ != 0);
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
  glUniform4f(color_location_, color_value, 0.0F, 1 - color_value, 1.0F);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kBarVertices);
}
