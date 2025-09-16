// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: MIT
//
// Implementation of AnalysisThread class.

#include "analysis_thread.h"

#include <thread>

#include "ring_buffer.h"

namespace {
constexpr size_t kDefaultCapacity = 2048;
}

AnalysisThread::AnalysisThread() {}

AnalysisThread::~AnalysisThread() {
  Stop();
}

bool AnalysisThread::Initialize() {
  if (!buffer_.Initialize(kDefaultCapacity)) {
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
    // TODO: Read ring buffer and write to FFTW.
    // TODO: Add analysis logic.
  }
}
