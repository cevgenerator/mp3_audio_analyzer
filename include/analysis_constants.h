// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Shared constants for audio analysis.

#pragma once

#include <cstddef>

namespace analysis {

constexpr size_t kChannels = 2;
constexpr size_t kFftSize = 512;
constexpr size_t kFftBinCount = kFftSize / kChannels;

}  // namespace analysis