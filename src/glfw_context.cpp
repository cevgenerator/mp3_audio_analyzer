// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of GlfwContext class.

#include "glfw_context.h"

#include <GLFW/glfw3.h>

#include "error_handling.h"
#include "window_constants.h"

GlfwContext::~GlfwContext() {
  if (window_ != nullptr) {
    glfwDestroyWindow(window_);
  }

  glfwTerminate();  // Safe even if initialization failed.
}

bool GlfwContext::Initialize() {
  if (!Succeeded("Initializing GLFW", (glfwInit() == GLFW_FALSE))) {
    return false;
  }

  window_ = glfwCreateWindow(window::kWindowWidth, window::kWindowHeight,
                             "MP3 Audio Analyzer", nullptr, nullptr);

  if (!Succeeded("Opening window", (window_ == nullptr))) {
    return false;
  }

  glfwMakeContextCurrent(window_);

  return true;
}

GLFWwindow* GlfwContext::window() {
  return window_;
}