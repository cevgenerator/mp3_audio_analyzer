// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AnalysisThread class.

#include "analysis_thread.h"

#include <cmath>
#include <iostream>

namespace {

// General audio settings
constexpr size_t kChannels = 2;
constexpr size_t kFftSize = 512;
constexpr size_t kFrameCount = kFftSize;
constexpr size_t kRingBufferCapacity = 4096;  // Enough for streaming.

// FFT-related constants
constexpr size_t kFftBinCount = kFftSize / 2;
constexpr float kFftSizeInverse = 1.0F / kFftSize;
constexpr float kEnergyThreshold = 0.1F;

}  // namespace

AnalysisThread::AnalysisThread() : interleaved_(kFftSize * kChannels) {}

AnalysisThread::~AnalysisThread() {
  Stop();
}

bool AnalysisThread::Initialize(long sample_rate) {
  sample_rate_ = static_cast<float>(sample_rate);  // For CalculateBandwidth().

  if (!buffer_.Initialize(kRingBufferCapacity)) {
    return false;
  }

  if (!fft.Initialize(kFftSize)) {
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

  for (size_t i = 0; i < kFftSize; i++) {
    rms_left += fft.input_left()[i] * fft.input_left()[i];
    rms_right += fft.input_right()[i] * fft.input_right()[i];
  }

  // Store results in data members.
  rms_left_ = std::sqrt(rms_left / kFftSize);
  rms_right_ = std::sqrt(rms_right / kFftSize);
}

// Calculates the frequency bandwidth for 1 channel.
// Is called by CalculateAverageBandwidth().
float AnalysisThread::CalculateBandwidth(const fftwf_complex* output) const {
  float min_freq = -1.0F;  // Sentinel value indicating uninitialized.
  float max_freq = -1.0F;

  // Only iterate through half the bins since FFT output is symmetric.
  for (size_t i = 0; i < kFftBinCount; ++i) {
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
// Must be called after fft.Execute().
void AnalysisThread::CalculateAverageBandwidth() {
  float bandwidth_left = CalculateBandwidth(fft.output_left());
  float bandwidth_right = CalculateBandwidth(fft.output_right());

  bandwidth_ = (bandwidth_left + bandwidth_right) / kChannels;
}

void AnalysisThread::Run() {
  while (running_) {
    // Read the ring buffer.
    // Skip and try again if not enough data is available.
    if (!buffer_.Pop(interleaved_.data(), kFftSize * kChannels)) {
      continue;  // Prevent old data is used again.
    }

    // Split the interleaved audio into two channels.
    for (size_t i = 0; i < kFrameCount; i++) {
      fft.input_left()[i] = interleaved_[2 * i];  // Copy each left sample.
      fft.input_right()[i] =
          interleaved_[(2 * i) + 1];  // Copy each right sample.
    }

    CalculateRms();

    // Perform the FFT.
    fft.Execute();

    CalculateAverageBandwidth();

    // Only print about twice per second.
    if (++fft_count_ % 43 == 0) {
      // Use the FFT output data before the loop runs again.
      // Print the second left and right bin to show FFTW is working.
      const auto* bin_left = fft.output_left()[1];
      const auto* bin_right = fft.output_right()[1];

      std::cout << "FFT_L[1]: Re = " << bin_left[0] << ", Im = " << bin_left[1]
                << "\n";
      std::cout << "FFT_R[1]: Re = " << bin_right[0]
                << ", Im = " << bin_right[1] << '\n';
      std::cout << "RMS_L: " << rms_left_ << '\n';
      std::cout << "RMS_R: " << rms_right_ << '\n';
      std::cout << "Bandwidth: " << bandwidth_ << "\n\n";
    }
  }
}
