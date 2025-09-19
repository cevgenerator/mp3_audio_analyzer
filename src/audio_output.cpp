// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Implementation of audio output classes using the PortAudio library.

#include "audio_output.h"

#include "error_handling.h"

// ---------------------------
// PortAudioSystem implementation
// ---------------------------

PortAudioSystem::PortAudioSystem() {
  error_ = Pa_Initialize();

  if (error_ == paNoError) {
    initialized_ = true;
  }
}

PortAudioSystem::~PortAudioSystem() {
  if (initialized_) {
    Pa_Terminate();
  }
}

int PortAudioSystem::error() const {
  return error_;
}

// ---------------------------
// AudioStream implementation
// ---------------------------

AudioStream::AudioStream(const PaStreamParameters& output_parameters,
                         long sample_rate) {
  constexpr unsigned long kFramesPerBuffer = 512;

  // Safe conversion of sample_rate_: MP3 sample rates are well below
  // precision limits of double.
  error_ = Pa_OpenStream(&stream_,
                         nullptr,  // No input.
                         &output_parameters, sample_rate, kFramesPerBuffer,
                         paClipOff,  // No clipping.
                         nullptr,    // No callback.
                         nullptr);   // No callback user data.
}

AudioStream::~AudioStream() {
  if (Pa_IsStreamActive(stream_) == 1) {
    Pa_StopStream(stream_);
  }

  if (stream_ != nullptr) {
    Pa_CloseStream(stream_);
  }
}

PaStream* AudioStream::stream() const {
  return stream_;
}
int AudioStream::error() const {
  return error_;
}

// ---------------------------
// AudioOutput implementation
// ---------------------------

AudioOutput::AudioOutput() {
  portaudio_error_ = audio_system_.error();
}

bool AudioOutput::Initialize(const Decoder& decoder) {
  return ValidateAudioSystem() && FindDefaultOutputDevice() &&
         ConfigureOutputParameters(decoder) && VerifyFormatSupport(decoder) &&
         OpenStream(decoder) && StartStream();
}

bool AudioOutput::WriteStream(const float* buffer, size_t frames) {
  if (!audio_stream_) return false;

  portaudio_error_ = Pa_WriteStream(audio_stream_->stream(), buffer, frames);

  return PortAudioSucceeded("Writing to output stream", portaudio_error_);
}

// Converts an mpg123 encoding format to a compatible PortAudio sample format.
//
// The input is the encoding value returned by mpg123_getformat().
PaSampleFormat AudioOutput::GetPortAudioFormat(int mpg123_encoding) {
  switch (mpg123_encoding) {
    case MPG123_ENC_SIGNED_16:
      return paInt16;
    case MPG123_ENC_SIGNED_8:
      return paInt8;
    case MPG123_ENC_UNSIGNED_8:
      return paUInt8;
    case MPG123_ENC_FLOAT_32:
      return paFloat32;
    default:
      return 0;  // Unsupported format.
  }
}

bool AudioOutput::ValidateAudioSystem() const {
  return PortAudioSucceeded("Validating PortAudio initialization",
                            portaudio_error_);
}

bool AudioOutput::FindDefaultOutputDevice() {
  output_parameters_.device = Pa_GetDefaultOutputDevice();

  return Succeeded("Finding default output device",
                   (output_parameters_.device == paNoDevice));
}

bool AudioOutput::ConfigureOutputParameters(const Decoder& decoder) {
  output_parameters_.channelCount = decoder.channels();
  output_parameters_.suggestedLatency =
      Pa_GetDeviceInfo(output_parameters_.device)->defaultLowOutputLatency;
  output_parameters_.hostApiSpecificStreamInfo = nullptr;
  output_parameters_.sampleFormat =
      GetPortAudioFormat(decoder.encoding_format());

  return Succeeded(
      "Verifying sample format compatibility between mpg123 and PortAudio",
      (output_parameters_.sampleFormat == 0));
}

// Check if the audio format is supported by the default output device.
//
// Safe conversion of sample_rate_: MP3 sample rates are well below precision
// limits of double.
bool AudioOutput::VerifyFormatSupport(const Decoder& decoder) {
  portaudio_error_ =
      Pa_IsFormatSupported(nullptr, &output_parameters_, decoder.sample_rate());

  return PortAudioSucceeded("Verifying audio format support by output device",
                            portaudio_error_);
}

bool AudioOutput::OpenStream(const Decoder& decoder) {
  audio_stream_.emplace(output_parameters_,
                        decoder.sample_rate());  // Calls constructor in-place.

  portaudio_error_ = audio_stream_->error();

  return PortAudioSucceeded("Opening PortAudio stream", portaudio_error_);
}

bool AudioOutput::StartStream() {
  if (!Succeeded("Validating audio stream initialization", (!audio_stream_))) {
    return false;
  }

  portaudio_error_ = Pa_StartStream(audio_stream_->stream());

  return PortAudioSucceeded("Starting PortAudio stream", portaudio_error_);
}
