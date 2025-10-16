// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declarations of shader utility functions.

#pragma once

#include <glad/gl.h>

#include <optional>
#include <string>

std::optional<std::string> LoadShaderSource(const std::string& path);
std::optional<GLuint> CompileShader(GLenum type, const std::string& source);
std::optional<GLuint> CreateShaderProgram(const std::string& vertex_path,
                                          const std::string& fragment_path);
