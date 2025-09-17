// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Declaration of FftwWrapper class. This class wraps the FFTW library for RAII
// and handles FFT initialization, execution, and provides access to FFT
// results.

#pragma once

#include <fftw3.h>

// Initialize() should be called right after the constructor.
class FftwWrapper {
 public:
  FftwWrapper();
  ~FftwWrapper();

  bool Initialize(size_t fft_size);
  void Execute();

  float* input_left();
  float* input_right();
  const fftwf_complex* output_left() const;
  const fftwf_complex* output_right() const;

 private:
  int fft_size_int = 0;
  float* input_left_ = nullptr;
  float* input_right_ = nullptr;
  fftwf_complex* output_left_ = nullptr;
  fftwf_complex* output_right_ = nullptr;
  fftwf_plan plan_left_ = nullptr;
  fftwf_plan plan_right_ = nullptr;
};