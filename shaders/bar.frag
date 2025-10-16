// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Fragment shader for FFT bar visualization.
// Outputs a solid color for each pixel.

#version 410 core  // OpenGL 4.1, core profile.

// Declare output color variable.
out vec4 frag_color;

// Declare input color uniform.
uniform vec4 color_uniform;

void main() {
    frag_color = color_uniform;
}
