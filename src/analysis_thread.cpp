// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AnalysisThread class.

#include "analysis_thread.h"

#include <cmath>

#include "analysis_constants.h"

namespace {

// General audio settings
constexpr size_t kFrameCount = analysis::kFftSize;
constexpr size_t kRingBufferCapacity = 4096;  // Enough for streaming.

// FFT-related constants
constexpr float kFftSizeInverse = 1.0F / analysis::kFftSize;
constexpr float kEnergyThreshold = 0.1F;

}  // namespace

AnalysisThread::AnalysisThread()
    : interleaved_(analysis::kFftSize * analysis::kChannels) {}

AnalysisThread::~AnalysisThread() {
  Stop();
}

bool AnalysisThread::Initialize(
    long sample_rate, const std::shared_ptr<AnalysisData>& analysis_data) {
  sample_rate_ = static_cast<float>(sample_rate);  // For CalculateBandwidth().
  analysis_data_ = analysis_data;

  if (!buffer_.Initialize(kRingBufferCapacity)) {
    return false;
  }

  if (!fft_.Initialize(analysis::kFftSize)) {
    return false;
  }

  Start();

  return true;
}

RingBuffer<float>& AnalysisThread::buffer() {
  return buffer_;
}

void AnalysisThread::Start() {
  running_ = true;
  thread_ = std::thread(&AnalysisThread::Run, this);
}

void AnalysisThread::Stop() {
  running_ = false;

  if (thread_.joinable()) {
    thread_.join();
  }
}

// Must be called after interleaved audio has been split.
void AnalysisThread::CalculateRms() {
  float rms_left = 0.0F;
  float rms_right = 0.0F;

  for (size_t i = 0; i < analysis::kFftSize; i++) {
    rms_left += fft_.input_left()[i] * fft_.input_left()[i];
    rms_right += fft_.input_right()[i] * fft_.input_right()[i];
  }

  rms_left = std::sqrt(rms_left / analysis::kFftSize);
  rms_right = std::sqrt(rms_right / analysis::kFftSize);

  rms_ = (rms_left + rms_right) / analysis::kChannels;
}

// Must be called after interleaved audio has been split.
void AnalysisThread::CalculateStereoCorrelation() {
  float correlation = 0.0F;

  for (size_t i = 0; i < analysis::kFftSize; ++i) {
    correlation += fft_.input_left()[i] * fft_.input_right()[i];
  }

  correlation_ = correlation * kFftSizeInverse;
}

// Calculates the frequency bandwidth for 1 channel.
// Is called by CalculateAverageBandwidth().
float AnalysisThread::CalculateBandwidth(const fftwf_complex* output) const {
  float min_freq = -1.0F;  // Sentinel value indicating uninitialized.
  float max_freq = -1.0F;

  // Only iterate through half the bins since FFT output is symmetric.
  for (size_t i = 0; i < analysis::kFftBinCount; ++i) {
    float real = output[i][0];
    float imaginary = output[i][1];
    float magnitude = std::sqrt((real * real) + (imaginary * imaginary));

    if (magnitude > kEnergyThreshold) {
      // Convert bin index to frequency.
      float freq = (static_cast<float>(i) * sample_rate_) * kFftSizeInverse;

      if (min_freq < 0) {
        min_freq = freq;
      }

      max_freq = freq;
    }
  }

  float bandwidth = max_freq - min_freq;

  return bandwidth;
}

// Calculates the average frequency bandwidth of 2 channels.
// Must be called after fft_.Execute().
void AnalysisThread::CalculateAverageBandwidth() {
  float bandwidth_left = CalculateBandwidth(fft_.output_left());
  float bandwidth_right = CalculateBandwidth(fft_.output_right());

  bandwidth_ = (bandwidth_left + bandwidth_right) / analysis::kChannels;
}

// Must be called after fft_.Execute().
void AnalysisThread::CalculateMagnitudes() {
  for (size_t i = 0; i < analysis::kFftBinCount; ++i) {
    float real_left = fft_.output_left()[i][0];
    float imaginary_left = fft_.output_left()[i][1];

    spectrum_left_[i] =
        std::sqrt((real_left * real_left) + (imaginary_left * imaginary_left));

    float real_right = fft_.output_right()[i][0];
    float imaginary_right = fft_.output_right()[i][1];

    spectrum_right_[i] = std::sqrt((real_right * real_right) +
                                   (imaginary_right * imaginary_right));
  }
}

void AnalysisThread::Run() {
  while (running_) {
    // Read the ring buffer.
    // Skip and try again if not enough data is available.
    if (!buffer_.Pop(interleaved_.data(),
                     analysis::kFftSize * analysis::kChannels)) {
      continue;  // Prevent old data is used again.
    }

    // Split the interleaved audio into two channels.
    for (size_t i = 0; i < kFrameCount; i++) {
      fft_.input_left()[i] = interleaved_[2 * i];  // Copy each left sample.
      fft_.input_right()[i] =
          interleaved_[(2 * i) + 1];  // Copy each right sample.
    }

    // Analyze audio.
    fft_.Execute();

    CalculateRms();
    CalculateStereoCorrelation();
    CalculateAverageBandwidth();
    CalculateMagnitudes();

    // Copy results to analysis_data.
    analysis_data_->Set(rms_, correlation_, bandwidth_, spectrum_left_,
                        spectrum_right_);
  }
}
