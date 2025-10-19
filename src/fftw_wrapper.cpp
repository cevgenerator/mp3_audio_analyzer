// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of FftwWrapper class.
//
// This wrapper is intended to be used as part of a real-time audio analysis
// pipeline where FFT is used for frequency analysis.

#include "fftw_wrapper.h"

FftwWrapper::~FftwWrapper() {
  fftwf_destroy_plan(plan_left_);
  fftwf_destroy_plan(plan_right_);
  fftwf_free(input_left_);
  fftwf_free(input_right_);
  fftwf_free(output_left_);
  fftwf_free(output_right_);
}

// Allocates memory for input/output buffers and creates FFTW plans.
bool FftwWrapper::Initialize(size_t fft_size) {
  fft_size_int = static_cast<int>(fft_size);
  input_left_ = (float*)fftwf_malloc(sizeof(float) * fft_size);
  input_right_ = (float*)fftwf_malloc(sizeof(float) * fft_size);
  output_left_ =
      (fftwf_complex*)fftwf_malloc(sizeof(fftw_complex) * (fft_size / 2 + 1));
  output_right_ =
      (fftwf_complex*)fftwf_malloc(sizeof(fftw_complex) * (fft_size / 2 + 1));

  if ((input_left_ == nullptr) || (input_right_ == nullptr) ||
      (output_left_ == nullptr) || (output_right_ == nullptr)) {
    return false;
  }

  plan_left_ = fftwf_plan_dft_r2c_1d(fft_size_int, input_left_, output_left_,
                                     FFTW_MEASURE);
  plan_right_ = fftwf_plan_dft_r2c_1d(fft_size_int, input_right_, output_right_,
                                      FFTW_MEASURE);

  return plan_left_ != nullptr && plan_right_ != nullptr;
}

// Performs the FFT.
void FftwWrapper::Execute() {
  fftwf_execute(plan_left_);
  fftwf_execute(plan_right_);
}

float* FftwWrapper::input_left() {
  return input_left_;
}

float* FftwWrapper::input_right() {
  return input_right_;
}

const fftwf_complex* FftwWrapper::output_left() const {
  return output_left_;
}

const fftwf_complex* FftwWrapper::output_right() const {
  return output_right_;
}
