// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Visualizer class.
//
// Uses OpenGL, GLAD, and GLFW to visualize audio analysis results via the
// Renderer class.
//
// Note: All method calls must be made from the main thread, as GLFW is not
// thread-safe.

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

  // Enters the main render loop. Exits when `running` is false.
  // Must only be called after Initialize().
  void Run(const std::atomic<bool>& running);

 private:
  GlfwContext glfw_;  // Manages GLFW window and OpenGL context.
  Renderer renderer;  // Responsible for rendering visual elements.
};
