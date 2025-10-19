// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of the AudioPipeline class.
//
// Manages real-time audio processing on a dedicated thread. Coordinates
// decoding, playback, and feeding data to the analysis thread.
//
// This class decouples audio I/O and decoding from the main thread, allowing
// rendering and visualization to remain responsive.
//
// After initialization, AudioPipeline assumes exclusive ownership of Decoder
// and AudioOutput usage. These must not be accessed from other threads after
// Start() is called.

#pragma once

#include <atomic>
#include <thread>

#include "analysis_thread.h"
#include "audio_output.h"
#include "decoder.h"

class AudioPipeline {
 public:
  AudioPipeline(Decoder& decoder, AudioOutput& audio_output,
                AnalysisThread& analysis_thread);
  ~AudioPipeline();

  // Class is not meant to be transferred or duplicated.
  AudioPipeline(const AudioPipeline&) = delete;
  AudioPipeline& operator=(const AudioPipeline&) = delete;
  AudioPipeline(AudioPipeline&&) = delete;
  AudioPipeline& operator=(AudioPipeline&&) = delete;

  // Starts the audio processing thread.
  void Start();

  // Returns whether the audio thread is still running.
  [[nodiscard]] const std::atomic<bool>& running() const;

 private:
  void Stop();
  void Run();

  Decoder& decoder_;
  AudioOutput& audio_output_;
  AnalysisThread& analysis_thread_;

  std::thread thread_;
  std::atomic<bool> running_ = false;
};
