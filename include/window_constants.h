// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Shared constants for audio visualization.

#pragma once

namespace window {

constexpr int kWindowWidth = 1280;
constexpr int kWindowHeight = 720;

constexpr float kPixelX = 2.0F / kWindowWidth;  // For NDC range: -1.0 to 1.0.
constexpr float kPixelY = 2.0F / kWindowHeight;

}  // namespace window