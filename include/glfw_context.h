// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of GlfwContext class. This class is an RAII wrapper for GLFW.
// Since GLFW is not thread-safe, all method calls must occur on the main
// thread.

#pragma once

// Prevent clangd/clang-format from reordering these.
// clang-format off
#include <glad/gl.h>
#include <GLFW/glfw3.h>
// clang-format on

class GlfwContext {
 public:
  GlfwContext() = default;
  ~GlfwContext();

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext& operator=(const GlfwContext&) = delete;

  [[nodiscard]] bool Initialize();

  [[nodiscard]] GLFWwindow* window();

 private:
  GLFWwindow* window_ = nullptr;
};