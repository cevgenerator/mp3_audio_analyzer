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

  return true;
}

void Visualizer::Run() {
  // TODO: Add visualization logic.
}

void Visualizer::Update() {
  analysis_data_->Get(rms_, correlation_, bandwidth_, spectrum_left_,
                      spectrum_right_);
}
