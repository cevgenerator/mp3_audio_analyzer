// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of Renderer class. Handles OpenGL state, bar geometry, and
// drawing.

#pragma once

#include <glad/gl.h>

class Renderer {
 public:
  Renderer();
  ~Renderer();

  bool Initialize();
  void Render() const;

 private:
  static bool InitializeOpenglState();
  bool CreateBarGeometry();

  GLuint shader_program_ = 0;
  GLuint vao_ = 0;
  GLuint vbo_ = 0;
};