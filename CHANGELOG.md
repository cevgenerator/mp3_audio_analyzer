# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.1.1] - 2025-08-30
### Refactor
- Refactored code for consistency (changed `frames` type to `size_t`)

### Style
- Improved code readability and error handling (e.g., separated variable declarations, replaced `NULL` with `nullptr`)

### Documentation
- Added comments explaining safe type conversion for MP3 sample rates
- Removed language from README.md title for brevity

## [0.1.0] - 2025-08-30
### Added
- Initial version with working MP3 playback via mpg123 and PortAudio
- Clean project structure and README
- CMake build system and custom find modules
- .clang-format and .gitignore setup
- .clang-tidy static analysis config
- License and CHANGELOG
- Adopted semantic versioning (SemVer) with project version tracking
- To-Do list entry for fixing audio glitches and investigating buffer underruns

#### Features
- Decodes MP3 to PCM and streams to default output device
- Implements real-time playback with error handling
- Uses dynamic buffer allocation based on MP3 format
- Includes setup validation and cleanup logic

