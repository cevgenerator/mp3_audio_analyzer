// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Visualizer class.
//
// This class uses OpenGL, GLAD and GLFW to visualize the results of the
// audio analysis (via Renderer). Since this class uses GLFW (which is not
// thread-safe), all method calls must occur on the main thread.

#pragma once

#include <memory>

#include "analysis_data.h"
#include "glfw_context.h"
#include "renderer.h"

class Visualizer {
 public:
  Visualizer() = default;
  ~Visualizer() = default;

  // Members are non-copyable and non-movable.
  Visualizer(const Visualizer&) = delete;
  Visualizer& operator=(const Visualizer&) = delete;
  Visualizer(Visualizer&&) = delete;
  Visualizer& operator=(Visualizer&&) = delete;

  // Initialize() must be called right after the constructor.
  [[nodiscard]] bool Initialize(
      long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data);
  void Run(const std::atomic<bool>& running);

 private:
  GlfwContext glfw_;
  Renderer renderer;
};