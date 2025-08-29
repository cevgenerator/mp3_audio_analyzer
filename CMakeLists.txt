# CMakeLists.txt for mp3_audio_analyzer
# This file configures the build system for the mp3 audio analyzer project.
# It sets up the C++ standard, finds dependencies, and defines the executable target.

cmake_minimum_required(VERSION 3.10)

# Project setup
project(mp3_audio_analyzer VERSION 0.1.0 LANGUAGES CXX)

# C++17 standard required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add custom modules path
list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# Find dependencies
find_package(MPG123 REQUIRED)
find_package(PortAudio REQUIRED)

# Source files
set(SOURCES
    src/main.cpp
)

# Build executable
add_executable(mp3_analyzer ${SOURCES})

# Link libraries
target_link_libraries(mp3_analyzer
    PRIVATE
        ${MPG123_LIBRARIES}
        ${PortAudio_LIBRARIES}
)

# Include headers
target_include_directories(mp3_analyzer
  PRIVATE
    ${MPG123_INCLUDE_DIRS}
    ${PortAudio_INCLUDE_DIRS}
)

