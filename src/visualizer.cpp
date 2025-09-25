// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of Visualizer class.

#include "visualizer.h"

#include <atomic>
#include <iostream>

#include "error_handling.h"
#include "window_constants.h"

namespace {

constexpr float kClearColorR = 0.0F;
constexpr float kClearColorG = 0.0F;
constexpr float kClearColorB = 0.0F;
constexpr float kClearColorA = 1.0F;

}  // namespace

Visualizer::Visualizer() {}
Visualizer::~Visualizer() {}

bool Visualizer::Initialize(
    const std::shared_ptr<AnalysisData>& analysis_data) {
  analysis_data_ = analysis_data;

  return glfw_.Initialize() && InitializeOpenglState();
}

// Must only be called after Initialize().
void Visualizer::Run(const std::atomic<bool>& running) {
  while (glfwWindowShouldClose(glfw_.window()) == GLFW_FALSE && running) {
    Update();

    // TODO: Add visualization logic.
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

bool Visualizer::InitializeOpenglState() const {
  // Safe conversion to bool.
  if (!Succeeded("Initializing GLAD",
                 (!gladLoadGL((GLADloadfunc)glfwGetProcAddress)))) {
    return false;
  }

  glViewport(0, 0, window::kWindowWidth, window::kWindowHeight);

  // Set background to black.
  glClearColor(kClearColorR, kClearColorG, kClearColorB, kClearColorA);

  // Enable blending.
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Enable depth testing.
  glEnable(GL_DEPTH_TEST);
  glDepthFunc(GL_LESS);

  return true;
}