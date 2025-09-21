# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/) and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [0.2.0] - 2025-09-21
- Added `FftwWrapper` class to encapsulate FFTW initialization and execution
- Refactored `AudioOutput` and `Decoder` classes to use `float` for FFTW compatibility
- Implemented analysis thread to enable FFT
- Improved FFT logic and demonstration output printing
- Updated project license to **GPL-2.0-or-later** to comply with FFTW’s license terms

## [0.1.6] - 2025-09-15
- Added reusable lock-free SPSC RingBuffer<T> class
- Added test for ring buffer under tests/
- Updated README with test instructions

## [v0.1.5] - 2025-09-10
### Fixed
- Audio glitches at startup and during volume changes resolved by setting `framesPerBuffer` to 512 instead of using `paFramesPerBufferUnspecified`

## [v0.1.4] – 2025-09-10
### Changed
- Renamed `Mpg123Decoder` to `Mpg123HandleWrapper` for clarity
- Refactored `Decoder` to encapsulate mpg123 setup and format logic
- Refactored `AudioOutput` to encapsulate PortAudio setup and streaming
- Moved and split decoder/audio output logic into separate files
- Styled improvements and consistency in comments
- Improved performance: enable `-O2` optimizations by default

## [0.1.3] - 2025-09-03
### Refactor
- Refactored error handling into helper functions
- Moved error handling to dedicated files

## [0.1.2] - 2025-09-02
### Refactor
- Centralized cleanup logic to reduce repetition and improve safety
- Introduced RAII wrappers for `mpg123_new()`, `Pa_Initialize()` and `Pa_OpenStream()`
- Refactored buffer allocation to use std::vector for RAII

### Style
- Internal code cleanup for clarity and consistency

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

