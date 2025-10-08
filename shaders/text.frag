// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Fragment shader for static text label rendering.
// Samples the alpha channel of a font atlas and outputs a modulated color.
// Assumes glyphs are white in the texture, using alpha for shape.

#version 410 core  // OpenGL 4.1, core profile.

// Input interpolated texture UV coordinates from vertex shader.
in vec2 interpolated_uv;

// Output final fragment color.
out vec4 frag_color;

// Uniforms set from C++ code.
uniform sampler2D font_texture;  // Font atlas texture sampler.
uniform vec4 color_uniform;  // Color to tint text with.

void main() {
    vec4 sampled = texture(font_texture, interpolated_uv);  // Sample font texture.
    frag_color = vec4(color_uniform.rgb, sampled.a * color_uniform.a);
}
