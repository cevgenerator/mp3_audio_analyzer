// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AudioPipeline class.
//
// Contains the threaded audio loop that decodes MP3 data, writes to the audio
// output, and pushes PCM data to the analysis thread.

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

  // Audio processing loop (runs on its own thread via AudioPipeline).
  // Continuously reads decoded PCM frames, pushes them to the analysis thread,
  // and writes them to the audio output stream.
  //
  // Runs until the MP3 is fully decoded or an error occurs.
  while (running_ && decoder_.Read(bytes_read)) {
    // The buffer contains bytes_read bytes of PCM data.
    size_t frames = bytes_read / decoder_.frame_size();

    // Push all interleaved samples (L+R) to the analysis buffer.
    // frames * 2 = total number of float samples (for stereo audio).
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
