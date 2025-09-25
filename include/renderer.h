// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Renderer class. Handles OpenGL state, bar geometry, and
// drawing.

#pragma once

class Renderer {
 public:
  Renderer();
  ~Renderer();

  static bool Initialize();
};