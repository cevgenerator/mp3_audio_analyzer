// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of GlfwContext class. This class is an RAII wrapper for GLFW.
// Since GLFW is not thread-safe, all method calls must occur on the main
// thread.

#pragma once

#include <GLFW/glfw3.h>

class GlfwContext {
 public:
  GlfwContext();
  ~GlfwContext();

  GlfwContext(const GlfwContext&) = delete;
  GlfwContext& operator=(const GlfwContext&) = delete;

  bool Initialize();

  GLFWwindow* window();

 private:
  GLFWwindow* window_ = nullptr;
};