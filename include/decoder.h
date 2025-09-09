#pragma once

#include <mpg123.h>

#include <cstddef>  // For size_t.
#include <vector>

// ----------------------
// Mpg123HandleWrapper class
// ----------------------

// Mpg123HandleWrapper is an RAII wrapper around mpg123_handle*
// that manages decoder creation and cleanup automatically.
class Mpg123HandleWrapper {
 public:
  Mpg123HandleWrapper();

  ~Mpg123HandleWrapper();

  mpg123_handle* handle() const;
  int error() const;

 private:
  int error_ = MPG123_OK;
  mpg123_handle* handle_ = nullptr;
};

// ----------------------
// Decoder class
// ----------------------

// Decoder is a wrapper around the mpg123 library that handles
// handle creation, file opening and reading, format extraction, and buffer
// allocation.
class Decoder {
 public:
  Decoder();

  // Initializes the decoder with the given MP3 file path.
  bool Initialize(const char* path);

  // Reads decoded PCM data into the internal buffer.
  bool Read(size_t& bytes_read);

  // Accessors.
  int mpg123_error() const;
  mpg123_handle* handle() const;
  long sample_rate() const;
  int channels() const;
  int encoding_format() const;
  const unsigned char* buffer_data() const;
  int frame_size() const;

 private:
  // Data members.
  int mpg123_error_ = MPG123_ERR;
  Mpg123HandleWrapper handle_wrapper_;
  mpg123_handle* handle_;  // A raw pointer from handle_wrapper_ (no ownership).

  long sample_rate_ = 0;
  int channels_ = 0;
  int encoding_format_ = 0;

  size_t buffer_size_ = 0;  // Set by AllocateBuffer(); 0 indicates failure.
  std::vector<unsigned char> buffer_;
  int bytes_per_sample_ =
      0;  // Set by DetermineBytesPerSample(); 0 indicates failure.
  int frame_size_ = 0;  // Set by DetermineFrameSize(); 0 indicates failure.

  // Internal helper functions.
  bool ValidateHandle() const;
  bool OpenFile(const char* path);
  bool GetFormatData();
  bool AllocateBuffer();
  bool DetermineBytesPerSample();
  bool DetermineFrameSize();
};
