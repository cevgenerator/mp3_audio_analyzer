// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Shared constants for audio analysis.
//
// kFftBinCount is the number of meaningful bins per channel after the FFT.

#pragma once

#include <cstddef>

namespace analysis {

constexpr size_t kChannels = 2;   // Stereo audio.
constexpr size_t kFftSize = 512;  // Must be power of two.
constexpr size_t kFftBinCount = kFftSize / 2;

}  // namespace analysis
