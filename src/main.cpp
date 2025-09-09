// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// MP3 Audio Player using mpg123 and PortAudio.
// Decodes an MP3 file to PCM and streams it in real-time.

#include <mpg123.h>
#include <portaudio.h>

#include <iostream>
#include <optional>
#include <string>
#include <vector>

#include "error_handling.h"

// ---------------------------
// Classes
// ---------------------------

// Mpg123HandleWrapper is an RAII wrapper around mpg123_handle*
// that manages decoder creation and cleanup automatically.
class Mpg123HandleWrapper {
 public:
  Mpg123HandleWrapper() { handle_ = mpg123_new(nullptr, &error_); }

  ~Mpg123HandleWrapper() {
    if (handle_ != nullptr) {
      mpg123_close(handle_);  // Closes the stream if it was opened.
      mpg123_delete(handle_);
    }
  }

  mpg123_handle* handle() const { return handle_; }
  int error() const { return error_; }

 private:
  int error_ = MPG123_OK;
  mpg123_handle* handle_ = nullptr;
};

// Decoder is a lightweight wrapper around the mpg123 library that handles
// handle creation, file opening and reading, format extraction, and buffer
// allocation.
class Decoder {
 public:
  Decoder() : handle_(handle_wrapper_.handle()) {}

  bool Initialize(const char* path) {
    return ValidateHandle() && OpenFile(path) && GetFormatData() &&
           AllocateBuffer() && DetermineBytesPerSample() &&
           DetermineFrameSize();
  }

  bool Read(size_t& bytes_read) {
    mpg123_error_ =
        mpg123_read(handle_, buffer_.data(), buffer_size_, &bytes_read);

    return Mpg123Succeeded("Reading MP3", mpg123_error_);
  }

  int mpg123_error() const { return mpg123_error_; }
  mpg123_handle* handle() const { return handle_; }
  long sample_rate() const { return sample_rate_; }
  int channels() const { return channels_; }
  int encoding_format() const { return encoding_format_; }
  const unsigned char* buffer_data() const { return buffer_.data(); }
  int frame_size() const { return frame_size_; }

 private:
  int mpg123_error_;
  Mpg123HandleWrapper handle_wrapper_;
  mpg123_handle* handle_;  // A raw pointer from handle_wrapper_ (no ownership).
  long sample_rate_;
  int channels_;
  int encoding_format_;
  size_t buffer_size_ = 0;
  std::vector<unsigned char> buffer_;
  int bytes_per_sample_ = 0;
  int frame_size_ = 0;

  bool ValidateHandle() const {
    return Succeeded("Validating mpg123 handle", (handle_ == nullptr));
  }

  bool OpenFile(const char* path) {
    mpg123_error_ = mpg123_open(handle_, path);

    return Mpg123Succeeded("Opening file", mpg123_error_);
  }

  bool GetFormatData() {
    mpg123_error_ =
        mpg123_getformat(handle_, &sample_rate_, &channels_, &encoding_format_);

    return Mpg123Succeeded("Retrieving format data", mpg123_error_);
  }

  bool AllocateBuffer() {
    buffer_size_ =
        mpg123_outblock(handle_);  // Get the recommended buffer size.

    buffer_.resize(buffer_size_);

    return Succeeded("Allocating buffer", (buffer_size_ == 0));
  }

  bool DetermineBytesPerSample() {
    bytes_per_sample_ = mpg123_encsize(encoding_format_);

    return Succeeded("Determining number of bytes per sample",
                     (bytes_per_sample_ == 0));
  }

  bool DetermineFrameSize() {
    frame_size_ = channels_ * bytes_per_sample_;

    return Succeeded("Determining frame size", (frame_size_ == 0));
  }
};

// PortAudioSystem is an RAII wrapper around Pa_Initialize()
// that manages audio system initialization and cleanup.
class PortAudioSystem {
 public:
  PortAudioSystem() {
    error_ = Pa_Initialize();

    if (error_ == paNoError) {
      initialized_ = true;
    }
  }

  ~PortAudioSystem() {
    if (initialized_) {
      Pa_Terminate();
    }
  }

  int error() const { return error_; }

 private:
  int error_ = paNoError;
  bool initialized_ = false;
};

// AudioStream is an RAII wrapper class around Pa_OpenStream()
// that manages audio stream opening and cleanup.
class AudioStream {
 public:
  AudioStream(const PaStreamParameters& output_parameters, long sample_rate) {
    // Open the audio stream.
    //
    // Safe conversion of sample_rate_: MP3 sample rates are well below
    // precision limits of double.
    error_ =
        Pa_OpenStream(&stream_,
                      nullptr,  // No input.
                      &output_parameters, sample_rate,
                      paFramesPerBufferUnspecified,  // Let PortAudio decide.
                      paClipOff,                     // No clipping.
                      nullptr,                       // No callback.
                      nullptr);                      // No callback user data.
  }

  ~AudioStream() {
    if (Pa_IsStreamActive(stream_) == 1) {
      Pa_StopStream(stream_);
    }

    if (stream_ != nullptr) {
      Pa_CloseStream(stream_);
    }
  }

  PaStream* stream() const { return stream_; }
  int error() const { return error_; }

 private:
  PaStream* stream_ = nullptr;
  int error_ = paNoError;
};

// AudioOutput is a wrapper around the PortAudio library that manages
// initialization and configuration, as well as stream opening, starting and
// writing.
class AudioOutput {
 public:
  AudioOutput() { portaudio_error_ = audio_system_.error(); }

  bool Initialize(const Decoder& decoder) {
    return ValidateAudioSystem() && FindDefaultOutputDevice() &&
           ConfigureOutputParameters(decoder) && VerifyFormatSupport(decoder) &&
           OpenStream(decoder) && StartStream();
  }

  bool WriteStream(const unsigned char* buffer, size_t frames) {
    if (!audio_stream_) return false;

    portaudio_error_ = Pa_WriteStream(audio_stream_->stream(), buffer, frames);

    return PortAudioSucceeded("Writing to output stream", portaudio_error_);
  }

 private:
  PortAudioSystem audio_system_;
  int portaudio_error_;
  PaStreamParameters output_parameters_;
  std::optional<AudioStream> audio_stream_;

  // Converts an mpg123 encoding format to a compatible PortAudio sample format.
  //
  // The input is the encoding value returned by mpg123_getformat().
  static PaSampleFormat GetPortAudioFormat(int mpg123_encoding) {
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

  bool ValidateAudioSystem() const {
    return PortAudioSucceeded("Validating PortAudio initialization",
                              portaudio_error_);
  }

  bool FindDefaultOutputDevice() {
    output_parameters_.device = Pa_GetDefaultOutputDevice();

    return Succeeded("Finding default output device",
                     (output_parameters_.device == paNoDevice));
  }

  bool ConfigureOutputParameters(const Decoder& decoder) {
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
  bool VerifyFormatSupport(const Decoder& decoder) {
    portaudio_error_ = Pa_IsFormatSupported(nullptr, &output_parameters_,
                                            decoder.sample_rate());

    return PortAudioSucceeded("Verifying audio format support by output device",
                              portaudio_error_);
  }

  bool OpenStream(const Decoder& decoder) {
    audio_stream_.emplace(
        output_parameters_,
        decoder.sample_rate());  // Calls constructor in-place.

    portaudio_error_ = audio_stream_->error();

    return PortAudioSucceeded("Opening PortAudio stream", portaudio_error_);
  }

  bool StartStream() {
    if (!Succeeded("Validating audio stream initialization",
                   (!audio_stream_))) {
      return false;
    }

    portaudio_error_ = Pa_StartStream(audio_stream_->stream());

    return PortAudioSucceeded("Starting PortAudio stream", portaudio_error_);
  }
};

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
