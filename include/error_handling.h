// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declarations of error checking helper functions.

#pragma once

#include <portaudio.h>

#include <string>

void LogError(const std::string& context, const std::string& message);

[[nodiscard]] bool Mpg123Succeeded(const std::string& context, int error);
[[nodiscard]] bool PortAudioSucceeded(const std::string& context,
                                      PaError error);
[[nodiscard]] bool Succeeded(const std::string& context, bool error);
