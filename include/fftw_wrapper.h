// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of FftwWrapper class.
//
// This class wraps the FFTW library for RAII and handles FFT initialization,
// execution, and provides access to FFT results.

#pragma once

#include <fftw3.h>

class FftwWrapper {
 public:
  FftwWrapper() = default;
  ~FftwWrapper();

  // Non-copyable and non-movable to pevent undefined behavior regarding raw
  // pointers and FFTW resources.
  FftwWrapper(const FftwWrapper&) = delete;
  FftwWrapper& operator=(const FftwWrapper&) = delete;
  FftwWrapper(FftwWrapper&& other) = delete;
  FftwWrapper& operator=(FftwWrapper&& other) = delete;

  // Initialize() must be called right after the constructor.
  [[nodiscard]] bool Initialize(size_t fft_size);

  // Executes the FFT operation on the input data.
  void Execute();

  [[nodiscard]] float* input_left();
  [[nodiscard]] float* input_right();
  [[nodiscard]] const fftwf_complex* output_left() const;
  [[nodiscard]] const fftwf_complex* output_right() const;

 private:
  int fft_size_int = 0;
  float* input_left_ = nullptr;
  float* input_right_ = nullptr;
  fftwf_complex* output_left_ = nullptr;
  fftwf_complex* output_right_ = nullptr;
  fftwf_plan plan_left_ = nullptr;
  fftwf_plan plan_right_ = nullptr;
};
