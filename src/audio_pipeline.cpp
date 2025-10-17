// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AudioPipeline class.

#include "audio_pipeline.h"

#include <cstddef>

AudioPipeline::AudioPipeline(Decoder& decoder, AudioOutput& audio_output,
                             AnalysisThread& analysis_thread)
    : decoder_(decoder),
      audio_output_(audio_output),
      analysis_thread_(analysis_thread) {}

AudioPipeline::~AudioPipeline() {
  Stop();
}

void AudioPipeline::Start() {
  running_ = true;
  thread_ = std::thread(&AudioPipeline::Run, this);
}

const std::atomic<bool>& AudioPipeline::running() const {
  return running_;
}

void AudioPipeline::Stop() {
  running_ = false;

  if (thread_.joinable()) {
    thread_.join();
  }
}

void AudioPipeline::Run() {
  size_t bytes_read;

  // This loop runs until the MP3 is fully decoded. The buffer contains
  // bytes_read bytes of PCM data.
  while (running_ && decoder_.Read(bytes_read)) {
    size_t frames = bytes_read / decoder_.frame_size();

    // Copy buffer to analysis thread.
    if (!analysis_thread_.buffer().Push(decoder_.buffer_data(), frames * 2)) {
      break;
    }

    // Copy buffer to audio output.
    if (!audio_output_.WriteStream(decoder_.buffer_data(), frames)) {
      break;
    }
  }

  running_ = false;  // Signal visualizer.
}