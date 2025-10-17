// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declarations for audio output functionality using the PortAudio library.
// Includes RAII wrappers for system and stream management, and a high-level
// AudioOutput interface that initializes, configures, and writes audio data.

#pragma once

#include <portaudio.h>

#include <cstddef>
#include <optional>

#include "decoder.h"

// ---------------------------
// PortAudioSystem class
// ---------------------------

// PortAudioSystem is an RAII wrapper for PortAudio initialization and
// termination.
class PortAudioSystem {
 public:
  PortAudioSystem();
  ~PortAudioSystem();

  [[nodiscard]] int error() const;

 private:
  int error_ = paNoError;
  bool initialized_ = false;
};

// ---------------------------
// AudioStream class
// ---------------------------

// AudioStream is an RAII wrapper for Pa_OpenStream() and Pa_CloseStream().
class AudioStream {
 public:
  AudioStream(const PaStreamParameters& output_parameters, long sample_rate);
  ~AudioStream();

  [[nodiscard]] PaStream* stream() const;
  [[nodiscard]] int error() const;

 private:
  PaStream* stream_ = nullptr;
  int error_ = paNoError;
};

// ---------------------------
// AudioOutput class
// ---------------------------

// AudioOutput is a high-level wrapper for audio playback using PortAudio.
// It handles system initialization, stream configuration, starting, and writing
// audio data.
class AudioOutput {
 public:
  AudioOutput();

  [[nodiscard]] bool Initialize(const Decoder& decoder);
  [[nodiscard]] bool WriteStream(const float* buffer, size_t frames);

 private:
  PortAudioSystem audio_system_;
  int portaudio_error_ = paNotInitialized;
  PaStreamParameters output_parameters_{};

  // audio_stream_ is constructed later when the necessary information is
  // available.
  std::optional<AudioStream> audio_stream_;

  // Internal methods

  [[nodiscard]] static PaSampleFormat GetPortAudioFormat(int mpg123_encoding);
  [[nodiscard]] bool ValidateAudioSystem() const;
  [[nodiscard]] bool FindDefaultOutputDevice();
  [[nodiscard]] bool ConfigureOutputParameters(const Decoder& decoder);
  [[nodiscard]] bool VerifyFormatSupport(const Decoder& decoder);
  [[nodiscard]] bool OpenStream(const Decoder& decoder);
  [[nodiscard]] bool StartStream();
};
