// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Vertex shader for static label rendering.
// Transforms quad vertices into clip space using a model and projection matrix.
// Passes UV coordinates to the fragment shader for texture sampling.

#version 410 core  // OpenGL 4.1, core profile.

// Define vertex input attributes.
layout(location = 0) in vec2 vertex_position;  // For glyph quad (NDC).
layout(location = 1) in vec2 vertex_uv;  // UV coordinate for sampling glyph.

// Output interpolated texture coordinate.
out vec2 interpolated_uv;

// Uniforms set from C++ code.
uniform mat4 model;  // Transforms object space to world space.
uniform mat4 projection;  // Positioning in world space.

void main() {
    // Pass UV coordinates to fragment shader.
    interpolated_uv = vertex_uv;

    // Compute final position of vertex in clip space.
    gl_Position = projection * model * vec4(vertex_position, 0.0, 1.0);
}
