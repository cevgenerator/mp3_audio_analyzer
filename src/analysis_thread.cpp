// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Implementation of AnalysisThread class.

#include "analysis_thread.h"

#include <iostream>

namespace {
constexpr size_t kChannels = 2;
constexpr size_t kFftSize = 512;
constexpr size_t kFrameCount = kFftSize;
constexpr size_t kRingBufferCapacity = kFftSize * 4;
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
    // Print just the first bin of the left channel (for testing).
    auto bin = fft.output_left()[0];
    std::cout << "FFT[0]: Re = " << bin[0] << ", Im = " << bin[1] << '\n';

    // TODO: Add analysis logic.
  }
}
