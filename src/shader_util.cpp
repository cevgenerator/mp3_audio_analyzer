// Copyright (c) 2025 Kars Helderman
// SPDX-License-Identifier: GPL-2.0-or-later
//
// Implementations of shader utility functions.
//
// Implements shader utility functions for loading, compiling, and linking GLSL
// shaders in OpenGL.

#include "shader_util.h"

#include <fstream>
#include <iostream>
#include <sstream>

namespace {

constexpr int kShaderInfoLogMaxLength = 512;

}  // namespace

// Loads the full contents of a shader source file into a string.
// Returns std::nullopt if the file can't be opened.
std::optional<std::string> LoadShaderSource(const std::string& path) {
  std::ifstream file(path);

  if (!file) {
    return std::nullopt;
  }

  std::stringstream buffer;

  buffer << file.rdbuf();

  return buffer.str();
}

// Compiles GLSL source code into a shader object.
// Returns shader ID on success, std::nullopt on failure.
std::optional<GLuint> CompileShader(GLenum type, const std::string& source) {
  GLuint shader = glCreateShader(type);  // Unique OpenGL ID.
  const char* src = source.c_str();      // Convert to raw C-style string.

  glShaderSource(shader, 1, &src, nullptr);  // Attach source.
  glCompileShader(shader);

  // Check compile status.
  GLint success = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

  if (success == 0) {
    // Needed for OpenGL C API call.
    // OpenGL expects a raw char* buffer to write the info log into.
    // NOLINTNEXTLINE(modernize-avoid-c-arrays)
    char info_log[kShaderInfoLogMaxLength];

    glGetShaderInfoLog(shader, sizeof(info_log), nullptr, info_log);

    std::cerr << "Shader compilation failed:\n" << info_log << '\n';

    glDeleteShader(shader);  // Always clean up.

    return std::nullopt;
  }

  return shader;
}

// Loads, compiles, and links a vertex and fragment shader into an OpenGL
// program. Returns program ID on success, std::nullopt on failure.
std::optional<GLuint> CreateShaderProgram(const std::string& vertex_path,
                                          const std::string& fragment_path) {
  auto vertex_source = LoadShaderSource(vertex_path);
  if (!vertex_source) {
    return std::nullopt;
  }

  auto fragment_source = LoadShaderSource(fragment_path);
  if (!fragment_source) {
    return std::nullopt;
  }

  auto vertex_shader = CompileShader(GL_VERTEX_SHADER, *vertex_source);
  if (!vertex_shader) {
    return std::nullopt;
  }

  auto fragment_shader = CompileShader(GL_FRAGMENT_SHADER, *fragment_source);
  if (!fragment_shader) {
    glDeleteShader(*vertex_shader);

    return std::nullopt;
  }

  // Create shader program object.
  GLuint program = glCreateProgram();  // Unique program ID.
  glAttachShader(program, *vertex_shader);
  glAttachShader(program, *fragment_shader);
  glLinkProgram(program);  // Create executable GPU program.

  GLint success;
  glGetProgramiv(program, GL_LINK_STATUS, &success);

  if (success == 0) {
    glDeleteShader(*vertex_shader);
    glDeleteShader(*fragment_shader);
    glDeleteProgram(program);

    return std::nullopt;
  }

  glDeleteShader(*vertex_shader);
  glDeleteShader(*fragment_shader);

  return program;
}
