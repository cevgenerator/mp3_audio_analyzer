# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.0] - 2025-08-29
### Added
- Initial version with working MP3 playback via mpg123 and PortAudio
- Clean project structure and README
- .clang-format and .gitignore setup
- CMake build system and custom find modules
- License and documentation files
- Project version tracking started

#### Features
- Decodes MP3 to PCM and streams to default output device
- Implements real-time playback with error handling
- Uses dynamic buffer allocation based on MP3 format
- Includes setup validation and cleanup logic

