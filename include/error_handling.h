// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of error checking helper functions.

#pragma once

#include <portaudio.h>

#include <string>

void LogError(const std::string& context, const std::string& message);

bool Mpg123Succeeded(const std::string& context, int error);
bool PortAudioSucceeded(const std::string& context, PaError error);
bool Succeeded(const std::string& context, bool error);
