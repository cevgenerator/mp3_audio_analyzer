// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// MP3 Audio Player using FFTW, mpg123 and PortAudio.
// Decodes an MP3 file to PCM and streams and analyses it in real-time.

#include <mpg123.h>
#include <portaudio.h>

#include <memory>

#include "analysis_data.h"
#include "analysis_thread.h"
#include "audio_output.h"
#include "audio_pipeline.h"
#include "decoder.h"
#include "visualizer.h"

int main() {
  // Create shared analysis data for communication between threads.
  auto analysis_data = std::make_shared<AnalysisData>();

  // Initialize decoder with input file.
  Decoder decoder;

  if (!decoder.Initialize(
          "../assets/quantum_jazz_orbiting_a_distant_planet_edit.mp3")) {
    return 1;
  }

  // Initialize audio output system.
  AudioOutput audio_output;

  if (!audio_output.Initialize(decoder)) {
    return 1;
  }

  // Initialize analysis thread.
  AnalysisThread analysis_thread;

  if (!analysis_thread.Initialize(decoder.sample_rate(), analysis_data)) {
    return 1;
  }

  // Initialize AudioPipeline.
  AudioPipeline audio_pipeline(decoder, audio_output, analysis_thread);

  audio_pipeline.Start();

  // Initialize visualizer.
  Visualizer visualizer;

  if (!visualizer.Initialize(decoder.sample_rate(), analysis_data)) {
    return 1;
  }

  visualizer.Run(audio_pipeline.running());

  return 0;
}
