// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Declarations of shader utility functions.
//
// These functions handle loading shader files, compiling GLSL source, and
// linking them into an OpenGL program.

#pragma once

#include <glad/gl.h>

#include <optional>
#include <string>

// Loads shader source code from a file.
[[nodiscard]] std::optional<std::string> LoadShaderSource(
    const std::string& path);

// Compiles GLSL source into a shader object.
[[nodiscard]] std::optional<GLuint> CompileShader(GLenum type,
                                                  const std::string& source);

// Creates a shader program from vertex and fragment shader file paths.
[[nodiscard]] std::optional<GLuint> CreateShaderProgram(
    const std::string& vertex_path, const std::string& fragment_path);
