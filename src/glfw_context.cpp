// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of GlfwContext class.

#include "glfw_context.h"

#include <GLFW/glfw3.h>

#include "error_handling.h"

namespace {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

}  // namespace

GlfwContext::GlfwContext() {}

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

  window_ = glfwCreateWindow(kWindowWidth, kWindowHeight, "MP3 Audio Analyzer",
                             nullptr, nullptr);

  if (window_ != nullptr) {
    glfwMakeContextCurrent(window_);
  }

  return Succeeded("Opening window", (window_ == nullptr));
}

GLFWwindow* GlfwContext::window() {
  return window_;
}