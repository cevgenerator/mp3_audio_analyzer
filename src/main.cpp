// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// MP3 Audio Player using mpg123 and PortAudio.
// Decodes an MP3 file to PCM and streams it in real-time.

#include <mpg123.h>
#include <portaudio.h>

#include <cstddef>  // For size_t.
#include <iostream>
#include <string>

#include "audio_output.h"
#include "decoder.h"
#include "error_handling.h"

int main() {
  // ---------------------------
  // Setup mpg123 and configure decoder
  // ---------------------------

  Decoder decoder;

  if (!decoder.Initialize("../assets/gradient_deep_performance_edit.mp3")) {
    return 1;
  }

  // ---------------------------
  // Configure and open output stream
  // ---------------------------

  AudioOutput audio_output;

  if (!audio_output.Initialize(decoder)) {
    return 1;
  }

  // ---------------------------
  // Decode and stream audio
  // ---------------------------

  size_t bytes_read;

  // Decode the MP3 into PCM and write it to the output stream.
  //
  // This loop runs until the MP3 is fully decoded. The buffer contains
  // bytes_read bytes of PCM data.
  while (decoder.Read(bytes_read)) {
    size_t frames = bytes_read / decoder.frame_size();

    if (!audio_output.WriteStream(decoder.buffer_data(), frames)) break;
  }

  // Check the reason the loop exited.
  if (decoder.mpg123_error() != MPG123_DONE &&
      !Mpg123Succeeded("Decoding", decoder.mpg123_error())) {
    return 1;
  }

  return 0;
}
