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

constexpr float kOrthoMin = -1.0F;
constexpr float kOrthoMax = 1.0F;

}  // namespace

bool Renderer::Initialize() {
  if (!Succeeded("Initializing OpenGL state", (InitializeOpenglState()))) {
    return false;
  }

  auto program = CreateShaderProgram("shaders/bar.vert", "shaders/bar.frag");

  if (!Succeeded("Creating shader program", (!program))) {
    return false;
  }

  shader_program_ = *program;

  // TODO: VAO/VBO setup

  return true;
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

  // Define coordinate system.
  glm::mat4 projection = glm::ortho(kOrthoMin, kOrthoMax,  // Left, right.
                                    kOrthoMin, kOrthoMax,  // Bottom, top.
                                    kOrthoMin, kOrthoMax   // Near, far.
  );

  return true;
}