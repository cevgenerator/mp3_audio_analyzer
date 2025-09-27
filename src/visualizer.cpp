// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Visualizer class.

#include "visualizer.h"

#include <atomic>
#include <iostream>

Visualizer::Visualizer() {}
Visualizer::~Visualizer() {}

bool Visualizer::Initialize(
    const std::shared_ptr<AnalysisData>& analysis_data) {
  analysis_data_ = analysis_data;

  return glfw_.Initialize() && renderer.Initialize();
}

// Must only be called after Initialize().
void Visualizer::Run(const std::atomic<bool>& running) {
  while (glfwWindowShouldClose(glfw_.window()) == GLFW_FALSE && running) {
    Update();

    renderer.Render();

    // Print metrics.
    std::cout << "rms: " << rms_ << '\n';
    std::cout << "corr: " << correlation_ << '\n';
    std::cout << "band: " << bandwidth_ << '\n';
    std::cout << "spec_l[1]: " << spectrum_left_[1] << '\n';
    std::cout << "spec_r[1]: " << spectrum_right_[1] << '\n';

    glfwSwapBuffers(glfw_.window());
    glfwPollEvents();
  }
}

void Visualizer::Update() {
  analysis_data_->Get(rms_, correlation_, bandwidth_, spectrum_left_,
                      spectrum_right_);
}
