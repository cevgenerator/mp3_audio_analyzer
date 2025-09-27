// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Vertex shader for FFT bar visualization.
// Transforms 2D vertex positions from model space to clip space
// using an orthographic projection.

#version 410 core  // OpenGL 4.1, core profile.

// Declare vertex position attribute.
layout(location = 0) in vec2 vertex_position;

// Declare projection matrix uniform.
uniform mat4 projection_matrix;

void main() {
    gl_Position = projection_matrix * vec4(vertex_position, 0.0, 1.0);
}
