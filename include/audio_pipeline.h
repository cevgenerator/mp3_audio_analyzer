// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of AudioPipeline class.
//
// This class encapsulates the real-time audio processing pipeline in a
// self-contained, threaded component. It manages decoding, audio playback, and
// feeding data to the analysis thread — all on a dedicated thread.
//
// By offloading these tasks, it ensures that main() remains responsive and free
// to run the visualization logic without being blocked by audio processing.

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

  void Start();

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