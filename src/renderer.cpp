// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Renderer class.

#include "renderer.h"

#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "error_handling.h"
#include "shader_util.h"
#include "window_constants.h"

namespace {

constexpr float kClearColorR = 0.0F;
constexpr float kClearColorG = 0.0F;
constexpr float kClearColorB = 0.0F;
constexpr float kClearColorA = 1.0F;

constexpr float kBarColorR = 0.2F;
constexpr int kBarVertices = 6;
constexpr float kBarWidth = 0.05F;
constexpr float kBarHeight = 0.8F;

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

bool Renderer::Initialize() {
  if (!Succeeded("Initializing OpenGL state", (!InitializeOpenglState()))) {
    return false;
  }

  auto program = CreateShaderProgram("shaders/bar.vert", "shaders/bar.frag");

  if (!Succeeded("Creating shader program", (!program))) {
    return false;
  }

  shader_program_ = *program;

  return Succeeded("Creating bar geometry", (!CreateBarGeometry()));
}

void Renderer::Render() const {
  // Clear screen before drawing new frame.
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  glUseProgram(shader_program_);
  glBindVertexArray(vao_);

  // Set the color uniform.
  GLint color_location = glGetUniformLocation(shader_program_, "color_uniform");
  glUniform4f(color_location, kBarColorR, 0.0F, 1.0F, 1.0F);  // Working color.

  // Set the projection matrix.
  GLint projection_location =
      glGetUniformLocation(shader_program_, "projection_matrix");
  glm::mat4 projection = glm::ortho(-1.0F, 1.0F, -1.0F, 1.0F);
  glUniformMatrix4fv(projection_location, 1, GL_FALSE, &projection[0][0]);

  // Draw 6 vertices (2 triangles).
  glDrawArrays(GL_TRIANGLES, 0, kBarVertices);

  glBindVertexArray(0);
  glUseProgram(0);
}

bool Renderer::InitializeOpenglState() {
  // Safe conversion to bool.
  if (!Succeeded("Initializing GLAD",
                 (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)))) {
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