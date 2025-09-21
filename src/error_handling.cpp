// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of error checking helper functions.

#include "error_handling.h"

#include <mpg123.h>

#include <iostream>

void LogError(const std::string& context, const std::string& message) {
  std::cerr << "[Error] " << context << ": " << message << '\n';
}

bool Mpg123Succeeded(const std::string& context, int error) {
  if (error != MPG123_OK) {
    LogError(context, mpg123_plain_strerror(error));
    return false;
  }
  return true;
}

bool PortAudioSucceeded(const std::string& context, PaError error) {
  if (error != paNoError) {
    LogError(context, Pa_GetErrorText(error));
    return false;
  }
  return true;
}

bool Succeeded(const std::string& context, bool error) {
  if (error) {
    LogError(context, "Failed.");
    return false;
  }
  return true;
}
