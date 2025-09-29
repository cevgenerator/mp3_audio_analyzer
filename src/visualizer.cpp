// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Visualizer class.

#include "visualizer.h"

#include <atomic>

Visualizer::Visualizer() {}
Visualizer::~Visualizer() {}

bool Visualizer::Initialize(
    long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data) {
  return glfw_.Initialize() && renderer.Initialize(sample_rate, analysis_data);
}

// Must only be called after Initialize().
void Visualizer::Run(const std::atomic<bool>& running) {
  while (glfwWindowShouldClose(glfw_.window()) == GLFW_FALSE && running) {
    renderer.Render();

    glfwSwapBuffers(glfw_.window());
    glfwPollEvents();
  }
}
