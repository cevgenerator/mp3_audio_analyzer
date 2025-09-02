// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// MP3 Audio Player using mpg123 and PortAudio.
// Decodes an MP3 file to PCM and streams it in real-time.

#include <mpg123.h>
#include <portaudio.h>

#include <iostream>
#include <vector>

// Mpg123Decoder is an RAII wrapper around mpg123_handle*
// that manages decoder creation and cleanup automatically.
class Mpg123Decoder {
 public:
  Mpg123Decoder() { handle_ = mpg123_new(nullptr, &error_); }

  ~Mpg123Decoder() {
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
  AudioStream(PaStreamParameters output_parameters, long sample_rate) {
    // Open the audio stream.
    //
    // Safe conversion of sample_rate: MP3 sample rates are well below precision
    // limits of double.
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

// Converts an mpg123 encoding format to a compatible PortAudio sample format.
//
// The input is the encoding value returned by mpg123_getformat().
PaSampleFormat GetPortAudioFormat(int mpg123_encoding) {
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

int main() {
  // ---------------------------
  // Setup mpg123 and configure decoder
  // ---------------------------

  // Create a new mpg123 handle.
  int mpg123_error;
  Mpg123Decoder decoder;

  auto* handle = decoder.handle();

  // Check if handle was created successfully.
  if (handle == nullptr) {
    std::cerr << "Failed to create handle. Error: "
              << mpg123_plain_strerror(decoder.error()) << "\n";

    return 1;
  }

  // Open the MP3 file.
  if (mpg123_open(handle, "../assets/gradient_deep_performance_edit.mp3") !=
      MPG123_OK) {
    std::cerr << "Failed to open file.\n";

    return 1;
  }

  // Get the format data needed to set the output format.
  long sample_rate;
  int channels;
  int encoding_format;

  mpg123_getformat(handle, &sample_rate, &channels, &encoding_format);

  // Allocate a buffer.
  size_t buffer_size;

  buffer_size = mpg123_outblock(handle);  // Get the recommended buffer size.
  std::vector<unsigned char> buffer(buffer_size);

  // ---------------------------
  // Configure and open output stream
  // ---------------------------

  // Initialize PortAudio library.
  PortAudioSystem audio_system;
  int portaudio_error = audio_system.error();

  if (portaudio_error == paNoError) {
    std::cout << "Portaudio initialized successfully.\n";
  } else {
    std::cerr << "Initializing PortAudio failed. Error: "
              << Pa_GetErrorText(portaudio_error)
              << " (code : " << portaudio_error << ")\n";

    return 1;
  };

  // Create a PortAudio output stream.
  PaStreamParameters output_parameters;

  output_parameters.device = Pa_GetDefaultOutputDevice();

  if (output_parameters.device == paNoDevice) {
    std::cerr << "No default output device.\n";

    return 1;
  }

  // Configure output parameters based on MP3 format.
  output_parameters.channelCount = channels;
  output_parameters.suggestedLatency =
      Pa_GetDeviceInfo(output_parameters.device)->defaultLowOutputLatency;
  output_parameters.hostApiSpecificStreamInfo = nullptr;
  output_parameters.sampleFormat = GetPortAudioFormat(encoding_format);

  // Verify sample format compatibility between mpg123 and PortAudio.
  if (output_parameters.sampleFormat == 0) {
    std::cerr << "Unsupported sample format for PortAudio.\n";

    return 1;
  }

  // Check if the audio format is supported by the default output device.
  //
  // Safe conversion of sample_rate: MP3 sample rates are well below precision
  // limits of double.
  if (Pa_IsFormatSupported(nullptr, &output_parameters, sample_rate) !=
      paFormatIsSupported) {
    std::cerr
        << "The audio format is not supported by the default output device.\n";

    return 1;
  }

  // ---------------------------
  // Open and start PortAudio stream
  // ---------------------------

  AudioStream audio_stream = AudioStream(output_parameters, sample_rate);
  portaudio_error = audio_stream.error();

  if (portaudio_error != paNoError) {
    std::cerr << "Failed to open PortAudio stream: "
              << Pa_GetErrorText(portaudio_error) << "\n";

    return 1;
  }

  // Start the audio stream.
  portaudio_error = Pa_StartStream(audio_stream.stream());
  if (portaudio_error != paNoError) {
    std::cerr << "Failed to start PortAudio stream: "
              << Pa_GetErrorText(portaudio_error) << "\n";

    return 1;
  }

  // ---------------------------
  // Decode and stream audio
  // ---------------------------

  size_t bytes_read;

  // Get bytes per sample for the encoding format.
  const int bytes_per_sample = mpg123_encsize(encoding_format);

  if (bytes_per_sample == 0) {
    std::cerr << "Unsupported encoding format.\n";

    return 1;
  }

  const int frame_size = channels * bytes_per_sample;

  // Decode the MP3 into PCM and write it to the output stream.
  //
  // This loop runs until the MP3 is fully decoded. The buffer contains
  // bytes_read bytes of PCM data.
  while ((mpg123_error = mpg123_read(handle, buffer.data(), buffer_size,
                                     &bytes_read)) == MPG123_OK) {
    size_t frames = bytes_read / frame_size;

    portaudio_error =
        Pa_WriteStream(audio_stream.stream(), buffer.data(), frames);

    if (portaudio_error != paNoError) {
      std::cerr << "PortAudio write error: " << Pa_GetErrorText(portaudio_error)
                << "\n";
      break;
    }
  }

  // Check the reason the loop exited.
  if (mpg123_error == MPG123_DONE) {
    std::cout << "Finished decoding successfully.\n";
  } else if (mpg123_error != MPG123_OK) {
    std::cerr << "Decoding failed. Error: " << mpg123_strerror(handle)
              << " (code " << mpg123_error << ")\n";

    return 1;
  }

  return 0;
}
