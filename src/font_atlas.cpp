// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementation of FontAtlas class.
//
// Intended to be used by Renderer to load a font texture and retrieve the UV
// coordinates for individual glyphs. The UV coordinates are used
// to map characters to specific portions of the texture atlas, which is then
// used for text rendering in OpenGL.

#include "font_atlas.h"

#include <unordered_map>

#include "error_handling.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"

FontAtlas::~FontAtlas() {
  if (texture_ != 0) {
    glDeleteTextures(1, &texture_);
  }
}

// Loads the font texture from a PNG file into an OpenGL texture.
//
// OpenGL context must be current before calling this method.
bool FontAtlas::LoadTexture() {
  int width;
  int height;
  int channels;  // RGBA.

  // Flip the PNG to match OpenGL's origin (lower-left).
  stbi_set_flip_vertically_on_load(1);

  unsigned char* data = nullptr;  // To hold raw RGBA values.
  data = stbi_load("../assets/font_atlas.png", &width, &height, &channels, 4);

  if (!Succeeded("Loading font texture", (data == nullptr))) {
    stbi_image_free(data);
    return false;
  }

  glGenTextures(1, &texture_);
  glBindTexture(GL_TEXTURE_2D, texture_);

  // Upload the texture pixel data to the GPU.
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA,
               GL_UNSIGNED_BYTE, data);

  stbi_image_free(data);

  // Set texture sampling and wrapping parameters.
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

  return true;
}

int FontAtlas::GetGlyphIndex(const std::string& character) {
  static const std::unordered_map<std::string, int> glyph_map = {
      {"A", 0},  {"H", 1},  {"L", 2},  {"M", 3},  {"R", 4},  {"S", 5},
      {"f", 6},  {"k", 7},  {"z", 8},  {"0", 9},  {"2", 10}, {"Δ", 11},
      {"𝜌", 12}, {"~", 13}, {"_", 14}, {" ", 15},
  };

  auto iterator = glyph_map.find(character);

  if (iterator != glyph_map.end()) {
    return iterator->second;  // Return index.
  }

  return font::kDefaultGlyphIndex;  // Default to space.
}

glm::vec4 FontAtlas::GetGlyphUv(const std::string& character) {
  int index = GetGlyphIndex(character);

  // Left edge of the glyph in UV space.
  float u_value =
      static_cast<float>(index * font::kGlyphWidth) / font::kAtlasWidth;

  // Top/bottom edge in UV space (depending on system).
  float v_value = 0.0F;

  // Normalized width and height of the glyph in UV space.
  float normalized_width = font::kGlyphWidth / font::kAtlasWidth;
  float normalized_height = font::kGlyphHeight / font::kAtlasHeight;

  return {u_value, v_value, u_value + normalized_width,
          v_value + normalized_height};
}

GLuint FontAtlas::texture() const {
  return texture_;  // ID used by Renderer to bind the texture for drawing.
}
