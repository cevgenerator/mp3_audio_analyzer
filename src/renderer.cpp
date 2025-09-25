// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Renderer class.

#include "renderer.h"

// Prevent clangd/clang-format from reordering these.
// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "error_handling.h"
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