// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Declaration of Decoder and Mpg123HandleWrapper classes,
// which provide a safe and easy interface for decoding MP3 files
// using the mpg123 library.

#pragma once

#include <mpg123.h>

#include <cstddef>  // For size_t.
#include <vector>

// ----------------------
// Mpg123HandleWrapper class
// ----------------------

// RAII wrapper for mpg123_handle*.
// Ensures handle is created and destroyed properly.
class Mpg123HandleWrapper {
 public:
  Mpg123HandleWrapper();

  ~Mpg123HandleWrapper();

  mpg123_handle* handle() const;
  int error() const;

 private:
  int error_ = MPG123_OK;
  mpg123_handle* handle_ = nullptr;  // Owned handle pointer.
};

// ----------------------
// Decoder class
// ----------------------

// Decoder wraps mpg123 and manages the full MP3 decoding pipeline.
// Handles file loading, format detection, PCM decoding, and buffer management.
class Decoder {
 public:
  Decoder();

  // Initializes the decoder with the given MP3 file path.
  bool Initialize(const char* path);

  // Reads decoded PCM data into the internal buffer.
  bool Read(size_t& bytes_read);

  // Accessors
  int mpg123_error() const;
  mpg123_handle* handle() const;
  long sample_rate() const;
  int channels() const;
  int encoding_format() const;
  const unsigned char* buffer_data() const;
  int frame_size() const;

 private:
  // Data members
  int mpg123_error_ = MPG123_ERR;
  Mpg123HandleWrapper handle_wrapper_;
  mpg123_handle* handle_;  // A raw pointer from handle_wrapper_ (no ownership).

  long sample_rate_ = 0;
  int channels_ = 0;
  int encoding_format_ = 0;

  size_t buffer_size_ = 0;             // 0 means allocation failure.
  std::vector<unsigned char> buffer_;  // PCM data buffer.
  int bytes_per_sample_ = 0;           // 0 indicates error.
  int frame_size_ = 0;                 // 0 indicates error.

  // Internal helper functions
  bool ValidateHandle() const;
  bool OpenFile(const char* path);
  bool GetFormatData();
  bool AllocateBuffer();
  bool DetermineBytesPerSample();
  bool DetermineFrameSize();
};
