// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of AnalysisThread class.

#include "analysis_thread.h"

#include <iostream>

namespace {
constexpr size_t kChannels = 2;
constexpr size_t kFftSize = 512;
constexpr size_t kFrameCount = kFftSize;
constexpr size_t kRingBufferCapacity =
    4096;  // Big enough for number of elements to be written.
}  // namespace

AnalysisThread::AnalysisThread() : interleaved_(kFftSize * kChannels) {}

AnalysisThread::~AnalysisThread() {
  Stop();
}

bool AnalysisThread::Initialize() {
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

void AnalysisThread::Run() {
  while (running_) {
    // Read the ring buffer.
    buffer_.Pop(interleaved_.data(), kFftSize * kChannels);

    // Split the interleaved audio into two channels.
    for (size_t i = 0; i < kFrameCount; i++) {
      fft.input_left()[i] = interleaved_[2 * i];  // Copy each left sample.
      fft.input_right()[i] =
          interleaved_[(2 * i) + 1];  // Copy each right sample.
    }

    // Perform the FFT.
    fft.Execute();

    // Use the FFT output data before the loop runs again.
    // Print the second left and right bin to show FFTW is working.
    auto bin_left = fft.output_left()[1];
    auto bin_right = fft.output_right()[1];

    std::cout << "FFT_L[1]: Re = " << bin_left[0] << ", Im = " << bin_left[1]
              << "\n";
    std::cout << "FFT_R[1]: Re = " << bin_right[0] << ", Im = " << bin_right[1]
              << "\n\n";

    // TODO: Add analysis logic.
  }
}
