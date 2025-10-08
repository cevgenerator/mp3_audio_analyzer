// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declaration of FontAtlas class. This class provides methods to the Renderer
// class for loading the font texture from font_atlas.png and determining the UV
// coordinates for each glyph.

#pragma once

#include <glad/gl.h>

#include <array>
#include <glm/glm.hpp>
#include <string>

namespace font {

constexpr int kGlyphWidth = 16;   // In pixels.
constexpr int kGlyphHeight = 32;  // In pixels.
constexpr int kAtlasCols = 16;
constexpr int kAtlasRows = 1;
constexpr float kAtlasWidth = 256.0F;  // Pixels.
constexpr float kAtlasHeight = 32.0F;  // Pixels.
constexpr int kDefaultGlyphIndex = 15;

const std::array<std::string, 10> kStaticLabels = {
    "A_L",     "A_R", "20 Hz", "20 Hz", "~22 kHz",
    "~22 kHz", "Δf",  "𝜌_LR",  "LR",    "RMS"};

const std::array<glm::vec2, kStaticLabels.size()> kLabelPositions = {
    glm::vec2(-0.96F, 0.5F),     // "A_L"
    glm::vec2(0.04F, 0.5F),      // "A_R"
    glm::vec2(-0.89F, 0.04F),    // "20 Hz" (L)
    glm::vec2(0.11F, 0.04F),     // "20 Hz" (R)
    glm::vec2(-0.223F, 0.04F),   // "~22 kHz" (L)
    glm::vec2(0.777F, 0.04F),    // "~22 kHz" (R)
    glm::vec2(-0.945F, -0.52F),  // "Δf"
    glm::vec2(-0.53F, -0.96F),   // "𝜌_LR"
    glm::vec2(0.48F, -0.96F),    // "LR"
    glm::vec2(0.04F, -0.52F),    // "RMS"
};

}  // namespace font

class FontAtlas {
 public:
  FontAtlas();
  ~FontAtlas();

  bool LoadTexture();
  static int GetGlyphIndex(const std::string& character);
  static glm::vec4 GetGlyphUv(const std::string& character);

  GLuint texture() const;

 private:
  GLuint texture_ = 0;
};