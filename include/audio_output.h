// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Declarations for audio output functionality using the PortAudio library.
// Includes RAII wrappers for system and stream management, and a high-level
// AudioOutput interface that initializes, configures, and writes audio data.

#pragma once

#include <portaudio.h>

#include <cstddef>  // For size_t.
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

  int error() const;

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

  PaStream* stream() const;
  int error() const;

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

  bool Initialize(const Decoder& decoder);

  bool WriteStream(const float* buffer, size_t frames);

 private:
  PortAudioSystem audio_system_;
  int portaudio_error_ = paNotInitialized;
  PaStreamParameters output_parameters_{};
  std::optional<AudioStream> audio_stream_;

  static PaSampleFormat GetPortAudioFormat(int mpg123_encoding);

  bool ValidateAudioSystem() const;

  bool FindDefaultOutputDevice();

  bool ConfigureOutputParameters(const Decoder& decoder);

  bool VerifyFormatSupport(const Decoder& decoder);

  bool OpenStream(const Decoder& decoder);

  bool StartStream();
};
