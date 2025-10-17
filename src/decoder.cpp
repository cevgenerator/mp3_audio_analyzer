// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of the Decoder and Mpg123HandleWrapper classes.
// These handle MPG123 library interactions, error checking, and buffer
// management.

#include "decoder.h"

#include "error_handling.h"

namespace {

constexpr long kSampleRate = 44100;

}  // namespace

// ----------------------
// Mpg123HandleWrapper implementation
// ----------------------

Mpg123HandleWrapper::Mpg123HandleWrapper()
    : handle_(mpg123_new(nullptr, &error_)) {}

Mpg123HandleWrapper::~Mpg123HandleWrapper() {
  if (handle_ != nullptr) {
    mpg123_close(handle_);  // Closes the stream if opened.
    mpg123_delete(handle_);
  }
}

mpg123_handle* Mpg123HandleWrapper::handle() const {
  return handle_;
}
int Mpg123HandleWrapper::error() const {
  return error_;
}

// ----------------------
// Decoder implementation
// ----------------------

Decoder::Decoder() : handle_(handle_wrapper_.handle()) {}

bool Decoder::Initialize(const char* path) {
  // Initialize the decoder step-by-step, abort on failure.
  return ValidateHandle() && OpenFile(path) && GetFormatData() &&
         AllocateBuffer() && DetermineBytesPerSample() && DetermineFrameSize();
}

// Decodes the next chunk of audio data into the internal buffer.
// Sets bytes_read to the number of PCM bytes written.
// Reinterpret buffer_ as unsigned char* so mpg123_read() can write raw PCM
// data. Assumes buffer_ is sized in bytes and stores float samples
// (MPG123_ENC_FLOAT_32).
bool Decoder::Read(size_t& bytes_read) {
  mpg123_error_ =
      mpg123_read(handle_, reinterpret_cast<unsigned char*>(buffer_.data()),
                  buffer_size_, &bytes_read);

  return Mpg123Succeeded("Reading MP3", mpg123_error_);
}

// Accessors

int Decoder::mpg123_error() const {
  return mpg123_error_;
}
mpg123_handle* Decoder::handle() const {
  return handle_;
}
long Decoder::sample_rate() const {
  return sample_rate_;
}
int Decoder::channels() const {
  return channels_;
}
int Decoder::encoding_format() const {
  return encoding_format_;
}
const float* Decoder::buffer_data() const {
  return buffer_.data();
}
int Decoder::frame_size() const {
  return frame_size_;
}

// Internal helper methods

bool Decoder::ValidateHandle() const {
  return Succeeded("Validating mpg123 handle", (handle_ == nullptr));
}

bool Decoder::OpenFile(const char* path) {
  mpg123_error_ = mpg123_open(handle_, path);

  return Mpg123Succeeded("Opening file", mpg123_error_);
}

// Sets decoding format to float.
bool Decoder::GetFormatData() {
  mpg123_format_none(handle_);
  mpg123_format(handle_, kSampleRate, MPG123_STEREO, MPG123_ENC_FLOAT_32);

  mpg123_error_ =
      mpg123_getformat(handle_, &sample_rate_, &channels_, &encoding_format_);

  return Mpg123Succeeded("Retrieving format data", mpg123_error_);
}

bool Decoder::AllocateBuffer() {
  buffer_size_ =
      mpg123_outblock(handle_);  // Get recommended buffer size in bytes.

  // Allocate float buffer: size in samples = bytes / sizeof(float)
  buffer_.resize(buffer_size_ / sizeof(float));

  return Succeeded("Allocating buffer", (buffer_size_ == 0));
}

bool Decoder::DetermineBytesPerSample() {
  bytes_per_sample_ = mpg123_encsize(encoding_format_);

  return Succeeded("Determining number of bytes per sample",
                   (bytes_per_sample_ == 0));
}

bool Decoder::DetermineFrameSize() {
  frame_size_ = channels_ * bytes_per_sample_;

  return Succeeded("Determining frame size", (frame_size_ == 0));
}
