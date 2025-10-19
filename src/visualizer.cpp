// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Visualizer class.
//
// This class runs the main visualization loop. It connects the audio analysis
// results to OpenGL rendering via the Renderer class and manages the GLFW
// window lifecycle through GlfwContext.
//
// Note: All method calls must be made from the main thread, as GLFW is not
// thread-safe.

#include "visualizer.h"

#include <atomic>

bool Visualizer::Initialize(
    long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data) {
  return glfw_.Initialize() && renderer.Initialize(sample_rate, analysis_data);
}

// Runs the main render loop.
void Visualizer::Run(const std::atomic<bool>& running) {
  while (glfwWindowShouldClose(glfw_.window()) == GLFW_FALSE && running) {
    // Render the current frame and handle window events.
    renderer.Render();

    glfwSwapBuffers(glfw_.window());
    glfwPollEvents();
  }
}
