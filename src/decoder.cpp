#include "decoder.h"

#include "error_handling.h"

// ----------------------
// Mpg123HandleWrapper class
// ----------------------

Mpg123HandleWrapper::Mpg123HandleWrapper()
    : handle_(mpg123_new(nullptr, &error_)) {}

Mpg123HandleWrapper::~Mpg123HandleWrapper() {
  if (handle_ != nullptr) {
    mpg123_close(handle_);  // Closes the stream if it was opened.
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
// Decoder class
// ----------------------

Decoder::Decoder() : handle_(handle_wrapper_.handle()) {}

bool Decoder::Initialize(const char* path) {
  return ValidateHandle() && OpenFile(path) && GetFormatData() &&
         AllocateBuffer() && DetermineBytesPerSample() && DetermineFrameSize();
}

bool Decoder::Read(size_t& bytes_read) {
  mpg123_error_ =
      mpg123_read(handle_, buffer_.data(), buffer_size_, &bytes_read);

  return Mpg123Succeeded("Reading MP3", mpg123_error_);
}

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
const unsigned char* Decoder::buffer_data() const {
  return buffer_.data();
}
int Decoder::frame_size() const {
  return frame_size_;
}

bool Decoder::ValidateHandle() const {
  return Succeeded("Validating mpg123 handle", (handle_ == nullptr));
}

bool Decoder::OpenFile(const char* path) {
  mpg123_error_ = mpg123_open(handle_, path);

  return Mpg123Succeeded("Opening file", mpg123_error_);
}

bool Decoder::GetFormatData() {
  mpg123_error_ =
      mpg123_getformat(handle_, &sample_rate_, &channels_, &encoding_format_);

  return Mpg123Succeeded("Retrieving format data", mpg123_error_);
}

bool Decoder::AllocateBuffer() {
  buffer_size_ = mpg123_outblock(handle_);  // Get the recommended buffer size.

  buffer_.resize(buffer_size_);

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
