// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Visualizer class.

#include "visualizer.h"

Visualizer::Visualizer() {}
Visualizer::~Visualizer() {}

bool Visualizer::Initialize(
    const std::shared_ptr<AnalysisData>& analysis_data) {
  analysis_data_ = analysis_data;

  return glfw_.Initialize();
}

// Must be called after Initialize().
void Visualizer::Run() {
  while (glfwWindowShouldClose(glfw_.window()) == GLFW_FALSE) {
    Update();

    // TODO: Add visualization logic.

    glfwSwapBuffers(glfw_.window());
    glfwPollEvents();
  }
}

void Visualizer::Update() {
  analysis_data_->Get(rms_, correlation_, bandwidth_, spectrum_left_,
                      spectrum_right_);
}
