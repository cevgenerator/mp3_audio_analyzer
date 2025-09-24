# CMakeLists.txt for mp3_audio_analyzer
# This file configures the build system for the mp3 audio analyzer project.
# It sets up the C++ standard, finds dependencies, and defines the executable target.

cmake_minimum_required(VERSION 3.10)

# Project setup
project(mp3_audio_analyzer VERSION 0.3.0 LANGUAGES C CXX)

# C++17 standard required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Default to Release if nothing is set
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Optimization for Release builds
set(CMAKE_CXX_FLAGS_RELEASE "-O2")

# Enable clang-tidy if installed
find_program(CLANG_TIDY_EXE NAMES "clang-tidy")
    if(CLANG_TIDY_EXE)
        message(STATUS "clang-tidy found: ${CLANG_TIDY_EXE}")
        set(CMAKE_CXX_CLANG_TIDY "${CLANG_TIDY_EXE}")
    endif()

# Add custom modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Set OpenGL policy for modern GLVND support
cmake_policy(SET CMP0072 NEW)

# Find dependencies
find_package(MPG123 REQUIRED)
find_package(OpenGL REQUIRED)
find_package(PortAudio REQUIRED)
find_package(PkgConfig REQUIRED)
pkg_check_modules(FFTW REQUIRED fftw3f)
pkg_check_modules(GLFW REQUIRED glfw3)

# Source files
set(SOURCES
    src/analysis_data.cpp
    src/analysis_thread.cpp
    src/audio_output.cpp
    src/audio_pipeline.cpp
    src/decoder.cpp
    src/error_handling.cpp
    src/fftw_wrapper.cpp
    src/glfw_context.cpp
    src/main.cpp
    src/visualizer.cpp
)

# Add GLAD as a static library
add_library(glad STATIC src/glad/gl.c)
target_include_directories(glad PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Build executable
add_executable(mp3_analyzer ${SOURCES})

# Link libraries
target_link_libraries(mp3_analyzer
    PRIVATE
        glad
        ${FFTW_LIBRARIES}
        ${GLFW_LIBRARIES}
        ${MPG123_LIBRARIES}
        ${PortAudio_LIBRARIES}
        OpenGL::GL
)

# Include headers
target_include_directories(mp3_analyzer
  PRIVATE
    ${FFTW_INCLUDE_DIRS}
    ${GLFW_INCLUDE_DIRS}
    ${MPG123_INCLUDE_DIRS}
    ${PortAudio_INCLUDE_DIRS}
    ${CMAKE_CURRENT_SOURCE_DIR}/include
)
